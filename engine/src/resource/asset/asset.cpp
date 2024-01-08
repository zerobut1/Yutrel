#include "yutrel_pch.hpp"

#include "asset.hpp"

#include "resource/asset/material.hpp"
#include "resource/asset/mesh.hpp"

#include <tiny_obj_loader.h>

#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace Yutrel
{
    Ref<Mesh> AssetManager::AddMesh(const std::string& path)
    {
        static uint32_t id = 1;
        if (!m_meshes.insert({id, CreateRef<Mesh>(path)}).second)
        {
            LOG_ERROR("Failed to add mesh");
            return nullptr;
        }

        return m_meshes[id++];
    }

    Ref<Material> AssetManager::AddMaterial(const Material& material)
    {
        static uint32_t id = 1;

        if (!m_materials.insert({id, CreateRef<Material>(material)}).second)
        {
            LOG_ERROR("Failed to add material");
            return nullptr;
        }

        return m_materials[id++];
    }

    void AssetManager::LoadFromFile(Ref<Mesh> mesh)
    {
        LOG_INFO("Load model from {}", mesh->path);
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

        std::unordered_map<Vertex, uint32_t> unique_vertices;

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex{};

                vertex.position = {
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
} // namespace Yutrel