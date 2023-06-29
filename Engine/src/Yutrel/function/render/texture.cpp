#include "yutrel_pch.hpp"

#include "texture.hpp"

#include "Yutrel/platform/OpenGL/opengl_texture.hpp"

namespace Yutrel
{
    Texture2D* Texture2D::Create(uint32_t width, uint32_t height)
    {
        return new OpenGLTexture2D(width, height);
    }

    Texture2D* Texture2D::Create(const std::string& path)
    {
        return new OpenGLTexture2D(path);
    }

    TextureCubemaps* TextureCubemaps::Create(const std::vector<std::string>& paths)
    {
        return new OpenGLTextureCubemaps(paths);
    }

    TextureCubemaps* TextureCubemaps::Create(const std::string& path)
    {
        return new OpenGLTextureCubemaps(path);
    }
} // namespace Yutrel