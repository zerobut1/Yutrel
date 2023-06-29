#pragma once
#include "Yutrel/function/render/mesh.hpp"

#include "Yutrel/platform/OpenGL/opengl_texture.hpp"
#include "Yutrel/platform/OpenGL/opengl_vertex_array.h"

#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

namespace Yutrel
{
    // constexpr int MAX_BONE_INFLUENCE = 4;

    class OpenGLMesh : public Mesh
    {
    public:
        OpenGLMesh(std::vector<float>& vertices, std::vector<uint32_t>& indices, std::vector<Texture*>& textures);

        virtual void Render() override;

    private:
        void setupMesh(std::vector<float>& vertices, std::vector<uint32_t>& indices, std::vector<Texture*>& textures);

    private:
        // 这样的格式不满足实时切换texture的需求,后面会更改
        std::vector<Texture*> textures;

        std::shared_ptr<VertexArray> m_VAO;
    };

} // namespace Yutrel
