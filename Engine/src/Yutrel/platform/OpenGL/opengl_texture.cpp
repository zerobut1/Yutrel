#include "yutrel_pch.h"

#include "opengl_texture.h"

#include "Yutrel/function/global/global_context.h"

#include <stb_image.h>

namespace Yutrel
{
    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
        : m_width(width), m_height(height)
    {
        m_internal_format = GL_RGB8;
        m_data_format     = GL_RGB;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
        glTextureStorage2D(m_rendererID, 1, m_internal_format, m_width, m_height);

        glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    OpenGLTexture2D::OpenGLTexture2D(const std::string &path)
        : m_path(path)
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        stbi_uc *data = nullptr;
        {
            data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        }
        // ENGINE_CORE_ASSERT(data, "Failed to load image!");
        m_width  = width;
        m_height = height;

        // 确认是RGB还是RGBA格式
        GLenum internalFormat = 0, dataFormat = 0;
        if (channels == 4)
        {
            internalFormat = GL_RGBA8;
            dataFormat     = GL_RGBA;
        }
        else if (channels == 3)
        {
            internalFormat = GL_RGB8;
            dataFormat     = GL_RGB;
        }
        m_internal_format = internalFormat;
        m_data_format     = dataFormat;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
        glTextureStorage2D(m_rendererID, 1, internalFormat, m_width, m_height);

        glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, dataFormat, GL_UNSIGNED_BYTE, data);

        glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        stbi_image_free(data);
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_rendererID);
    }

    void OpenGLTexture2D::setData(void *data, uint32_t size)
    {
        uint32_t bpp = m_data_format == GL_RGBA ? 4 : 3;
        // ENGINE_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");

        glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_data_format, GL_UNSIGNED_BYTE, data);
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_rendererID);
    }

    /**
     *这里的实现是比较早的opengl版本的实现
     *后面可能会更新
     */
    OpenGLTextureCubemaps::OpenGLTextureCubemaps(const std::vector<std::string> &paths)
        : m_paths(paths)
    {
        // glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_rendererID);
        glGenTextures(1, &m_rendererID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_rendererID);

        for (unsigned int i = 0; i < paths.size(); i++)
        {
            int width, height, channels;
            stbi_uc *data = stbi_load(paths[i].c_str(), &width, &height, &channels, 0);

            m_width  = width;
            m_height = height;

            // 确认是RGB还是RGBA格式
            GLenum internalFormat = 0, dataFormat = 0;
            if (channels == 4)
            {
                internalFormat = GL_RGBA8;
                dataFormat     = GL_RGBA;
            }
            else if (channels == 3)
            {
                internalFormat = GL_RGB8;
                dataFormat     = GL_RGB;
            }

            m_internal_format = internalFormat;
            m_data_format     = dataFormat;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    OpenGLTextureCubemaps::~OpenGLTextureCubemaps()
    {
        glDeleteTextures(1, &m_rendererID);
    }

    void OpenGLTextureCubemaps::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_rendererID);
    }
} // namespace Yutrel