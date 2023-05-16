#include "mesh.h"

#include "runtime/platform/OpenGL/opengl_mesh.h"

#include <memory>

namespace Yutrel
{
    std::shared_ptr<Mesh> Mesh::create(std::vector<float> &vertices, std::vector<uint32_t> &indices, std::vector<std::shared_ptr<Texture>> &textures)
    {
        return std::make_shared<OpenGLMesh>(vertices, indices, textures);
    }
} // namespace Yutrel