#include "opengl_mesh.h"

#include <memory>
#include <stdint.h>
#include <vector>

namespace Yutrel
{
    OpenGLMesh::OpenGLMesh(std::vector<float> &vertices, std::vector<uint32_t> &indices, std::vector<std::shared_ptr<Texture>> &textures)
    {
        this->vertices = &vertices[0];
        this->indices  = &indices[0];
        this->textures = textures;

        setupMesh();
    }

    void OpenGLMesh::setupMesh()
    {
        VAO                               = VertexArray::create();
        std::shared_ptr<VertexBuffer> VBO = VertexBuffer::create(vertices, sizeof(vertices));
        VBO->setLayout({{Yutrel::ShaderDataType::Float3, "a_Pos"},
                        {Yutrel::ShaderDataType::Float3, "a_Normal"},
                        {Yutrel::ShaderDataType::Float2, "a_TexCoord"}});
        VAO->addVertexBuffer(VBO);
        std::shared_ptr<IndexBuffer> EBO = IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t));
        VAO->setIndexBuffer(EBO);
    }

    void OpenGLMesh::Draw()
    {
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            textures[i]->Bind(i);
        }
        VAO->Bind();
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(sizeof(indices) / sizeof(uint32_t)), GL_UNSIGNED_INT, 0);
        VAO->Unbind();
    }

} // namespace Yutrel