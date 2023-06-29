#pragma once

#include "Yutrel/function/render/texture.hpp"

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
        OpenGLTextureCubemaps(const std::string &path);
        virtual ~OpenGLTextureCubemaps();

        virtual void Bind(uint32_t slot = 0) const override;

        virtual uint32_t getWidth() const override { return m_width; }
        virtual uint32_t getHeight() const override { return m_height; }

        // 暂时不知道有啥用 先不实现了
        virtual void setData(void *data, uint32_t size) override{};

    private:
        // std::vector<std::string> m_paths;
        uint32_t m_width, m_height;
        uint32_t m_rendererID;
        GLenum m_internal_format, m_data_format;
    };
} // namespace Yutrel