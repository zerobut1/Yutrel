#include "yutrel_pch.hpp"

#include "texture.h"

#include "Yutrel/platform/OpenGL/opengl_texture.h"

namespace Yutrel
{
    std::shared_ptr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        return std::make_shared<OpenGLTexture2D>(width, height);
    }

    std::shared_ptr<Texture2D> Texture2D::Create(const std::string &path)
    {
        return std::make_shared<OpenGLTexture2D>(path);
    }

    std::shared_ptr<TextureCubemaps> TextureCubemaps::Create(const std::vector<std::string> &paths)
    {
        return std::make_shared<OpenGLTextureCubemaps>(paths);
    }

    std::shared_ptr<TextureCubemaps> TextureCubemaps::Create(const std::string &path)
    {
        return std::make_shared<OpenGLTextureCubemaps>(path);
    }
} // namespace Yutrel