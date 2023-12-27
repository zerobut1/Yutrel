#include "yutrel_pch.hpp"

#include "asset.hpp"

#include <tiny_obj_loader.h>

#include <string>
#include <vector>

namespace Yutrel
{
    Mesh& AssetManager::LoadMesh(const std::string& path)
    {
        m_meshes.insert({path, Mesh(path)});

        return m_meshes[path];
    }

    bool AssetManager::LoadFromFile(Mesh& mesh)
    {
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

                    new_vert.uv.x = ux;
                    new_vert.uv.y = 1 - uy;

                    // 将颜色设为法线
                    new_vert.color = new_vert.normal;

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