#include "yutrel_pch.hpp"

#include "mesh.hpp"

#include "Yutrel/platform/OpenGL/opengl_mesh.hpp"

namespace Yutrel
{
    Mesh* Mesh::Create(std::vector<float>& vertices, std::vector<uint32_t>& indices, std::vector<Texture*>& textures)
    {
        return new OpenGLMesh(vertices, indices, textures);
    }
} // namespace Yutrel