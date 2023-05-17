#pragma once
#include "runtime/function/render/mesh.h"

#include "runtime/platform/OpenGL/opengl_texture.h"
#include "runtime/platform/OpenGL/opengl_vertex_array.h"

#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

namespace Yutrel
{
    constexpr int MAX_BONE_INFLUENCE = 4;

    struct Vertex
    {
        // position
        glm::vec3 Position;
        // normal
        glm::vec3 Normal;
        // texCoords
        glm::vec2 TexCoords;
    };

    class OpenGLMesh : public Mesh
    {
    public:
        OpenGLMesh(std::vector<float> &vertices, std::vector<uint32_t> &indices, std::vector<std::shared_ptr<Texture>> &textures);

        // render the mesh
        virtual void Draw() override;

    private:
        void setupMesh(std::vector<float> &vertices, std::vector<uint32_t> &indices, std::vector<std::shared_ptr<Texture>> &textures);

    private:
        // float *vertices;
        std::vector<float> vertices;
        // uint32_t *indices;
        std::vector<uint32_t> indices;
        std::vector<std::shared_ptr<Texture>> textures;

        std::shared_ptr<VertexArray> VAO;
    };

} // namespace Yutrel
