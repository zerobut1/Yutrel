#include "yutrel_pch.hpp"

#include "asset.hpp"

#include "resource/asset/gltf_scene.hpp"
#include "resource/asset/material.hpp"
#include "resource/asset/mesh.hpp"
#include "resource/asset/texture.hpp"
#include "resource/component/component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <tiny_gltf.h>
#include <tiny_obj_loader.h>

#include <stdint.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vcruntime.h>
#include <vector>

namespace Yutrel
{
    Ref<Mesh> AssetManager::AddMesh(const std::string& path)
    {
        return m_meshes.emplace_back(CreateRef<Mesh>(path));
    }

    Ref<Material> AssetManager::AddMaterial(const Material& material)
    {
        return m_materials.emplace_back(CreateRef<Material>(material));
    }

    Ref<Texture> AssetManager::AddTexture(const std::string& path)
    {
        return m_textures.emplace_back(CreateRef<Texture>(path));
    }

    Ref<GLTFScene> AssetManager::AddGLTFScene(const std::string& path)
    {
        return m_gltf_scenes.emplace_back(CreateRef<GLTFScene>(path));
    }

    void AssetManager::LoadFromFile(Ref<Mesh> mesh)
    {
        LOG_INFO("Load mesh from {}", mesh->path);
        mesh->vertices = CreateRef<std::vector<Vertex>>();
        mesh->indices  = CreateRef<std::vector<uint32_t>>();

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        // 加载模型
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, mesh->path.c_str()))
        {
            LOG_ERROR(warn + err);
            return;
        }

        std::unordered_map<Vertex, uint32_t> unique_vertices{};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex{};

                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2],
                };

                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2],
                };

                vertex.uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0 - attrib.texcoords[2 * index.texcoord_index + 1],
                };

                if (!unique_vertices.count(vertex))
                {
                    unique_vertices[vertex] = static_cast<uint32_t>(mesh->vertices->size());
                    mesh->vertices->push_back(vertex);
                }
                mesh->indices->push_back(unique_vertices[vertex]);
            }
        }

        mesh->is_loaded = true;
    }

    void AssetManager::LoadFromFile(Ref<Texture> texture)
    {
        LOG_INFO("Load image from {}", texture->path);

        texture->image = CreateRef<Image>();

        // 从文件中读取
        texture->image->pixels = stbi_load(texture->path.c_str(), &texture->image->width, &texture->image->height, &texture->image->channels, STBI_rgb_alpha);

        if (!texture->image->pixels)
        {
            LOG_ERROR("Failed to load image from {}", texture->path);
        }

        texture->is_loaded = true;
    }

    void AssetManager::LoadFromFile(gecs::entity scene_entity, Ref<GLTFScene> scene, const Transform& transform, Children& children, gecs::commands& cmd)
    {
        LOG_INFO("Load scene from {}", scene->path);

        tinygltf::Model model;
        tinygltf::TinyGLTF loader;
        std::string error, warning;

        bool file_loaded = loader.LoadASCIIFromFile(&model, &error, &warning, scene->path);

        size_t pos       = scene->path.find_last_of('/');
        scene->root_path = scene->path.substr(0, pos + 1);

        if (file_loaded)
        {
            // 加载所有的图片
            std::vector<Ref<Texture>> images;
            for (size_t i = 0; i < model.images.size(); i++)
            {
                tinygltf::Image& gltf_image = model.images[i];
                images.emplace_back(AddTexture(scene->root_path + gltf_image.uri));
            }

            // 暂时只加载文件中的第一个场景
            const tinygltf::Scene& gltf_scene = model.scenes[0];
            for (size_t i = 0; i < gltf_scene.nodes.size(); i++)
            {
                const tinygltf::Node gltf_node = model.nodes[gltf_scene.nodes[i]];

                // // 每个SceneBundle的entity中，保存若干个子entity，即gltf的node
                // auto node = cmd.create();

                // // 将node保存至scene的子节点中
                // children.entities.emplace_back(node);

                // 加载每个node的primitive，以及子node
                // Children node_children;
                LoadNode(gltf_node, model, transform, images, &scene_entity, children, cmd);

                // // 每个node的子entity
                // cmd.emplace_bundle(node, NodeBundle{
                //                              transform,
                //                              std::move(node_children),
                //                              Primitives{},
                //                          });
            }
        }
        else
        {
            LOG_ERROR("Failed to load gltf scene from {}", scene->path);
        }

        scene->is_loaded = true;
    }

    void AssetManager::LoadNode(const tinygltf::Node& in_node,
                                const tinygltf::Model& model,
                                const Transform& transform,
                                const std::vector<Ref<Texture>>& images,
                                gecs::entity* parent,
                                Children& children,
                                gecs::commands& cmd)
    {
        auto node = cmd.create();
        // 父节点
        if (parent != nullptr)
        {
            cmd.emplace<Parent>(node, *parent);
            children.entities.emplace_back(node);
        }

        // 模型矩阵
        auto matrix = glm::mat4(1.0f);

        if (in_node.translation.size() == 3)
        {
            matrix = glm::translate(matrix, glm::vec3(glm::make_vec3(in_node.translation.data())));
        }
        if (in_node.rotation.size() == 4)
        {
            glm::quat q = glm::make_quat(in_node.rotation.data());
            matrix *= glm::mat4(q);
        }
        if (in_node.scale.size() == 3)
        {
            matrix = glm::scale(matrix, glm::vec3(glm::make_vec3(in_node.scale.data())));
        }
        if (in_node.matrix.size() == 16)
        {
            matrix = glm::make_mat4x4(in_node.matrix.data());
        };

        matrix = transform.model_matrix * matrix;

        // 加载子节点
        if (in_node.children.size() > 0)
        {
            Children node_children{};
            for (size_t i = 0; i < in_node.children.size(); i++)
            {
                LoadNode(model.nodes[in_node.children[i]], model, transform, images, &node, node_children, cmd);
            }
            cmd.emplace<Children>(node, node_children);
        }

        // primitives
        if (in_node.mesh > -1)
        {
            const tinygltf::Mesh gltf_mesh = model.meshes[in_node.mesh];
            for (size_t i = 0; i < gltf_mesh.primitives.size(); i++)
            {
                auto primitive = cmd.create();
                cmd.emplace<Transform>(primitive, matrix);

                Ref<Mesh> mesh = AddMesh("");
                mesh->vertices = CreateRef<std::vector<Vertex>>();
                mesh->indices  = CreateRef<std::vector<uint32_t>>();

                const tinygltf::Primitive& gltf_primitive = gltf_mesh.primitives[i];
                uint32_t vertex_start                     = static_cast<uint32_t>(mesh->vertices->size());

                // 顶点
                {
                    const float* position_buffer = nullptr;
                    const float* normals_buffer  = nullptr;
                    const float* uv_buffer       = nullptr;
                    const float* tangents_buffer = nullptr;
                    size_t vertex_count          = 0;

                    // 位置坐标
                    if (gltf_primitive.attributes.find("POSITION") != gltf_primitive.attributes.end())
                    {
                        const tinygltf::Accessor& accessor = model.accessors[gltf_primitive.attributes.find("POSITION")->second];
                        const tinygltf::BufferView& view   = model.bufferViews[accessor.bufferView];
                        position_buffer                    = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                        vertex_count                       = accessor.count;
                    }
                    // 法线
                    if (gltf_primitive.attributes.find("NORMAL") != gltf_primitive.attributes.end())
                    {
                        const tinygltf::Accessor& accessor = model.accessors[gltf_primitive.attributes.find("NORMAL")->second];
                        const tinygltf::BufferView& view   = model.bufferViews[accessor.bufferView];
                        normals_buffer                     = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    }
                    // 切线
                    if (gltf_primitive.attributes.find("TANGENT") != gltf_primitive.attributes.end())
                    {
                        const tinygltf::Accessor& accessor = model.accessors[gltf_primitive.attributes.find("TANGENT")->second];
                        const tinygltf::BufferView& view   = model.bufferViews[accessor.bufferView];
                        tangents_buffer                    = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    }
                    // uv
                    // 暂时只加载第一组uv
                    if (gltf_primitive.attributes.find("TEXCOORD_0") != gltf_primitive.attributes.end())
                    {
                        const tinygltf::Accessor& accessor = model.accessors[gltf_primitive.attributes.find("TEXCOORD_0")->second];
                        const tinygltf::BufferView& view   = model.bufferViews[accessor.bufferView];
                        uv_buffer                          = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    }

                    // 加载到mesh的顶点数据中
                    for (size_t v = 0; v < vertex_count; v++)
                    {
                        Vertex vert{};
                        vert.pos     = glm::vec4(glm::make_vec3(&position_buffer[v * 3]), 1.0f);
                        vert.normal  = glm::normalize(glm::vec3(normals_buffer ? glm::make_vec3(&normals_buffer[v * 3]) : glm::vec3(0.0f)));
                        vert.tangent = tangents_buffer ? glm::make_vec4(&tangents_buffer[v * 4]) : glm::vec4(0.0f);
                        vert.uv      = uv_buffer ? glm::make_vec2(&uv_buffer[v * 2]) : glm::vec3(0.0f);

                        mesh->vertices->push_back(vert);
                    }
                }
                // 索引
                {
                    const tinygltf::Accessor& accessor      = model.accessors[gltf_primitive.indices];
                    const tinygltf::BufferView& buffer_view = model.bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer          = model.buffers[buffer_view.buffer];

                    switch (accessor.componentType)
                    {
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                    {
                        const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + buffer_view.byteOffset]);
                        for (size_t index = 0; index < accessor.count; index++)
                        {
                            mesh->indices->push_back(buf[index] + vertex_start);
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                    {
                        const uint16_t* buf = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + buffer_view.byteOffset]);
                        for (size_t index = 0; index < accessor.count; index++)
                        {
                            mesh->indices->push_back(buf[index] + vertex_start);
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                    {
                        const uint8_t* buf = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + buffer_view.byteOffset]);
                        for (size_t index = 0; index < accessor.count; index++)
                        {
                            mesh->indices->push_back(buf[index] + vertex_start);
                        }
                        break;
                    }
                    default:
                        LOG_ERROR("Index component type {} not supported!", accessor.componentType);
                        return;
                    }
                }
                // mesh加入到primitive中
                mesh->is_loaded = true;
                cmd.emplace<Ref<Mesh>>(primitive, mesh);

                // 材质
                Material material{};
                tinygltf::Material gltf_material = model.materials[gltf_primitive.material];
                {
                    if (gltf_material.values.find("baseColorFactor") != gltf_material.values.end())
                    {
                        material.base_color = glm::make_vec4(gltf_material.values["baseColorFactor"].ColorFactor().data());
                    }
                    if (gltf_material.values.find("baseColorTexture") != gltf_material.values.end())
                    {
                        material.base_color_texture = images[gltf_material.values["baseColorTexture"].TextureIndex()];
                    }
                    // Get the normal map texture index
                    // if (gltf_material.additionalValues.find("normalTexture") != gltf_material.additionalValues.end())
                    // {
                    //     material.normalTextureIndex = gltf_material.additionalValues["normalTexture"].TextureIndex();
                    // }
                }
                cmd.emplace<Ref<Material>>(primitive, AddMaterial(std::move(material)));
            }
        }
    }

    void AssetManager::LoadMeshes(gecs::querier<Ref<Mesh>> meshes,
                                  gecs::resource<gecs::mut<AssetManager>> asset_manager)
    {
        for (const auto& [entity, mesh] : meshes)
        {
            if (!mesh->is_loaded)
            {
                asset_manager->LoadFromFile(mesh);
            }
        }
    }

    void AssetManager::LoadMaterials(gecs::querier<Ref<Material>> materials,
                                     gecs::resource<gecs::mut<AssetManager>> asset_manager)
    {
        for (const auto& [entity, material] : materials)
        {
            // 基础色纹理
            if (!material->base_color_texture->is_loaded)
            {
                asset_manager->LoadFromFile(material->base_color_texture);
            }
        }
    }

    void AssetManager::LoadGLTFScenes(gecs::querier<Ref<GLTFScene>, Transform, gecs::mut<Children>> scenes,
                                      gecs::resource<gecs::mut<AssetManager>> asset_manager,
                                      gecs::commands cmd)
    {
        for (const auto& [entity, scene, transform, children] : scenes)
        {
            if (!scene->is_loaded)
            {
                asset_manager->LoadFromFile(entity, scene, transform, children, cmd);
            }
        }
    }

} // namespace Yutrel