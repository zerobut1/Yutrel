#include "yutrel_pch.hpp"

#include "asset.hpp"

#include <stdint.h>
#include <tiny_obj_loader.h>

#include <string>
#include <vector>

namespace Yutrel
{
    Mesh& AssetManager::TempAddMesh()
    {
        std::vector<Vertex> rect_vertices(4);

        rect_vertices[0].position = {0.5, -0.5, 0};
        rect_vertices[1].position = {0.5, 0.5, 0};
        rect_vertices[2].position = {-0.5, -0.5, 0};
        rect_vertices[3].position = {-0.5, 0.5, 0};

        rect_vertices[0].color = {0, 0, 0, 1};
        rect_vertices[1].color = {0.5, 0.5, 0.5, 1};
        rect_vertices[2].color = {1, 0, 0, 1};
        rect_vertices[3].color = {0, 1, 0, 1};

        std::vector<uint32_t> rect_indices(6);

        rect_indices[0] = 0;
        rect_indices[1] = 1;
        rect_indices[2] = 2;

        rect_indices[3] = 2;
        rect_indices[4] = 1;
        rect_indices[5] = 3;

        Mesh mesh;
        mesh.vertices = CreateRef<std::vector<Vertex>>(rect_vertices);
        mesh.indices  = CreateRef<std::vector<uint32_t>>(rect_indices);

        m_meshes.insert({"NULL", mesh});

        return m_meshes["NULL"];
    }

    Mesh& AssetManager::LoadMesh(const std::string& path)
    {
        m_meshes.insert({path, Mesh(path)});

        return m_meshes[path];
    }

    bool AssetManager::LoadFromFile(Mesh& mesh)
    {
        LOG_INFO("Load model from {}", mesh.path);
        mesh.vertices = CreateRef<std::vector<Vertex>>();

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        // 加载模型
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, mesh.path.c_str()))
        {
            std::cout << (warn + err) << std::endl;
            return false;
        }

        // 将网格放入顶点缓冲区
        for (size_t s = 0; s < shapes.size(); s++)
        {
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
            {
                int fv = 3;
                for (size_t v = 0; v < fv; v++)
                {
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                    // pos
                    tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                    // normal
                    tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                    tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                    tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
                    // uv
                    tinyobj::real_t ux = attrib.texcoords[2 * idx.texcoord_index + 0];
                    tinyobj::real_t uy = attrib.texcoords[2 * idx.texcoord_index + 1];

                    // 数据转化为顶点
                    Vertex new_vert;
                    new_vert.position.x = vx;
                    new_vert.position.y = vy;
                    new_vert.position.z = vz;

                    new_vert.normal.x = nx;
                    new_vert.normal.y = ny;
                    new_vert.normal.z = nz;

                    // new_vert.uv.x = ux;
                    // new_vert.uv.y = 1 - uy;

                    // // 将颜色设为法线
                    // new_vert.color = new_vert.normal;

                    mesh.vertices->push_back(new_vert);
                }
                index_offset += fv;
            }
        }

        mesh.is_loaded = true;
        return true;
    }

    // Asset::Asset(Asset&& other) noexcept
    //     : m_id{std::exchange(other.m_id, 0)}
    // {
    // }

    // Asset& Asset::operator=(Asset&& other) noexcept
    // {
    //     if (this != &other)
    //     {
    //         this->m_id = std::exchange(other.m_id, 0);
    //     }
    //     return *this;
    // }
} // namespace Yutrel