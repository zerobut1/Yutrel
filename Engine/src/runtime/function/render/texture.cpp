#include "texture.h"

#include "runtime/platform/OpenGL/opengl_texture.h"

#include <memory>

namespace Yutrel
{
    std::shared_ptr<Texture2D> Texture2D::create(uint32_t width, uint32_t height)
    {
        return std::make_shared<OpenGLTexture2D>(width, height);
    }

    std::shared_ptr<Texture2D> Texture2D::create(const std::string &path)
    {
        return std::make_shared<OpenGLTexture2D>(path);
    }
} // namespace Yutrel