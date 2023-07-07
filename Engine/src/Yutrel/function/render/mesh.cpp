#include "yutrel_pch.hpp"

#include "mesh.hpp"


#include <glad/glad.h>

namespace Yutrel
{
    Mesh* Mesh::Create(std::vector<float>& vertices, std::vector<uint32_t>& indices, std::vector<Material*>& materials)
    {
        return new Mesh(vertices, indices, materials);
    }

    Mesh::Mesh(std::vector<float>& vertices, std::vector<uint32_t>& indices, std::vector<Material*>& materials)
    {
        this->m_materials = materials;

        m_VAO             = VertexArray::Create();
        VertexBuffer* VBO = VertexBuffer::Create(&vertices[0], vertices.size() * sizeof(float));
        VBO->SetLayout({{Yutrel::ShaderDataType::Float3, "a_Pos"},
                        {Yutrel::ShaderDataType::Float3, "a_Normal"},
                        {Yutrel::ShaderDataType::Float3, "a_Tangent"},
                        {Yutrel::ShaderDataType::Float3, "a_Bittangent"},
                        {Yutrel::ShaderDataType::Float2, "a_TexCoord"}});
        m_VAO->AddVertexBuffer(VBO);
        IndexBuffer* EBO = IndexBuffer::Create(&indices[0], indices.size());
        m_VAO->SetIndexBuffer(EBO);
    }

} // namespace Yutrel