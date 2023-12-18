#include "yutrel_pch.hpp"

#include "mesh.hpp"

#include <glad/glad.h>
#include <memory>
#include <stdint.h>

namespace Yutrel
{
    Ref<Mesh> Mesh::Create(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
    {
        return CreateRef<Mesh>(vertices, indices);
    }

    Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
    {
        // VAO
        m_vertex_array = VertexArray::Create();
        auto VBO       = VertexBuffer::Create(&vertices[0], vertices.size() * sizeof(Vertex));
        VBO->SetLayout({{Yutrel::ShaderDataType::Float3, "a_position"},
                        {Yutrel::ShaderDataType::Float3, "a_normal"},
                        {Yutrel::ShaderDataType::Float3, "a_tangent"},
                        {Yutrel::ShaderDataType::Float3, "a_bitangent"},
                        {Yutrel::ShaderDataType::Float2, "a_uv"},
                        {Yutrel::ShaderDataType::Int4, "a_bone_ids"},
                        {Yutrel::ShaderDataType::Float4, "a_bone_weights"}});
        m_vertex_array->AddVertexBuffer(VBO);
        auto EBO = IndexBuffer::Create(&indices[0], indices.size());
        m_vertex_array->SetIndexBuffer(EBO);

        // AABB
        glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
        for (auto&& vertex : vertices)
        {
            minAABB.x = std::min(minAABB.x, vertex.position.x);
            minAABB.y = std::min(minAABB.y, vertex.position.y);
            minAABB.z = std::min(minAABB.z, vertex.position.z);

            maxAABB.x = std::max(maxAABB.x, vertex.position.x);
            maxAABB.y = std::max(maxAABB.y, vertex.position.y);
            maxAABB.z = std::max(maxAABB.z, vertex.position.z);
        }

        m_AABB = std::make_unique<AABB>(minAABB, maxAABB);
    }

    void Mesh::SetMaterialName(std::string& name) const
    {
        m_material_name = name;
    }
} // namespace Yutrel