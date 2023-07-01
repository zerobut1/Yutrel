#include "yutrel_pch.hpp"

#include "opengl_mesh.hpp"

namespace Yutrel
{
    OpenGLMesh::OpenGLMesh(std::vector<float>& vertices, std::vector<uint32_t>& indices, std::vector<Texture*>& textures)
    {
        this->textures = textures;
        SetupMesh(vertices, indices, textures);
    }

    void OpenGLMesh::SetupMesh(std::vector<float>& vertices, std::vector<uint32_t>& indices, std::vector<Texture*>& textures)
    {
        m_VAO             = VertexArray::Create();
        VertexBuffer* VBO = VertexBuffer::Create(&vertices[0], vertices.size() * sizeof(float));
        VBO->setLayout({{Yutrel::ShaderDataType::Float3, "a_Pos"},
                        {Yutrel::ShaderDataType::Float3, "a_Normal"},
                        {Yutrel::ShaderDataType::Float2, "a_TexCoord"}});
        m_VAO->addVertexBuffer(VBO);
        IndexBuffer* EBO = IndexBuffer::Create(&indices[0], indices.size());
        m_VAO->setIndexBuffer(EBO);
    }

    void OpenGLMesh::Draw()
    {
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            textures[i]->Bind(i + 1);
        }
        m_VAO->Bind();

        glDrawElements(GL_TRIANGLES, m_VAO->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, 0);

        m_VAO->Unbind();
    }

} // namespace Yutrel