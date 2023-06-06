#include "yutrel_pch.h"

#include "mesh.h"

#include "Yutrel/platform/OpenGL/opengl_mesh.h"

namespace Yutrel
{
    std::shared_ptr<Mesh> Mesh::Create(std::vector<float> &vertices, std::vector<uint32_t> &indices, std::vector<std::shared_ptr<Texture>> &textures)
    {
        return std::make_shared<OpenGLMesh>(vertices, indices, textures);
    }
} // namespace Yutrel