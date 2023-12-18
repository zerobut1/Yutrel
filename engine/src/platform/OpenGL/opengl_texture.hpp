#pragma once

#include "resource/asset/texture.hpp"

#include <glad/glad.h>

#include <stdint.h>
#include <string>
#include <vector>

namespace Yutrel
{

    class OpenGLTexture : public Texture
    {
    public:
        static void Copy(const Ref<OpenGLTexture> fr, uint32_t fr_level, const Ref<OpenGLTexture> to, uint32_t to_level);

    public:
        OpenGLTexture(const std::string& img_path, bool flip, uint32_t levels);
        OpenGLTexture(const std::string& img_path, uint32_t resolution, uint32_t levels);
        // OpenGLTexture(const std::string& directory, const std::string& extension, uint32_t resolution, uint32_t levels);
        OpenGLTexture(GLenum target, uint32_t width, uint32_t height, uint32_t depth, GLenum i_format, uint32_t levels);
        virtual ~OpenGLTexture();

        virtual void Bind(uint32_t index) const override;
        virtual void Unbind(uint32_t index) const override;

        virtual void SetData(const void* data) const override;

        virtual uint32_t GetRendererID() const override { return m_id; }
        virtual uint32_t GetWidth() const override { return m_width; }
        virtual uint32_t GetHeight() const override { return m_height; }
        virtual uint32_t GetLevels() const override { return m_levels; }
        virtual GLenum GetInternalFormat() const override { return m_internal_format; }

    public:
        virtual void BindILS(uint32_t level, uint32_t index, GLenum access) const override;
        virtual void UnbindILS(uint32_t index) const override;

        virtual void GenerateMipmap() const override;
        virtual void Clear(uint32_t level) const override;
        virtual void Invalidate(uint32_t level) const override;

    private:
        void SetSampleState() const;

    private:
        uint32_t m_width, m_height, m_depth;
        uint32_t m_levels;
        GLenum m_target;
        GLenum m_format, m_internal_format;
    };
} // namespace Yutrel