#pragma once

#include "runtime/function/render/texture.h"

#include <glad/glad.h>

#include <stdint.h>
#include <string>
#include <vector>

namespace Yutrel
{
    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(uint32_t width, uint32_t height);
        OpenGLTexture2D(const std::string &path);
        virtual ~OpenGLTexture2D();

        virtual void Bind(uint32_t slot = 0) const override;

        virtual uint32_t getWidth() const override { return m_width; }
        virtual uint32_t getHeight() const override { return m_height; }

        virtual void setData(void *data, uint32_t size) override;

        //--------tmp---------
        virtual uint32_t getRnedererID() const override { return m_rendererID; };
        virtual void setSize(uint32_t width, uint32_t height) override
        {
            // glTextureSubImage2D(m_rendererID, 0, 0, 0, width, height, m_data_format, GL_UNSIGNED_BYTE, nullptr);
            // glTexImage2D(m_rendererID, 0, m_data_format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
            glTextureStorage2D(m_rendererID, 0, m_internal_format, width, height);
        }

    private:
        std::string m_path;
        uint32_t m_width, m_height;
        uint32_t m_rendererID;
        GLenum m_internal_format, m_data_format;
    };

    class OpenGLTextureCubemaps : public TextureCubemaps
    {
    public:
        OpenGLTextureCubemaps(const std::vector<std::string> &paths);
        virtual ~OpenGLTextureCubemaps();

        virtual void Bind(uint32_t slot = 0) const override;

        virtual uint32_t getWidth() const override { return m_width; }
        virtual uint32_t getHeight() const override { return m_height; }

        // 暂时不知道有啥用 先不实现了
        virtual void setData(void *data, uint32_t size) override{};

    private:
        std::vector<std::string> m_paths;
        uint32_t m_width, m_height;
        uint32_t m_rendererID;
        GLenum m_internal_format, m_data_format;
    };
} // namespace Yutrel