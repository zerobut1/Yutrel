#include "yutrel_pch.h"

#include "texture.h"

#include "runtime/platform/OpenGL/opengl_texture.h"

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

    std::shared_ptr<TextureCubemaps> TextureCubemaps::create(const std::vector<std::string> &paths)
    {
        return std::make_shared<OpenGLTextureCubemaps>(paths);
    }
} // namespace Yutrel