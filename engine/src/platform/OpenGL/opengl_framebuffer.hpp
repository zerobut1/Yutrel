#pragma once

#include "resource/asset/framebuffer.hpp"

#include "core/macro.hpp"
#include "resource/asset/texture.hpp"

#include "glm/glm.hpp"
#include <glad/glad.h>

#include <memory>
#include <stdint.h>

namespace Yutrel
{
    class OpenGLRenderBuffer : public RenderBuffer
    {
    public:
        OpenGLRenderBuffer(uint32_t width, uint32_t height, bool multisample = false);
        virtual ~OpenGLRenderBuffer();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual uint32_t GetRendererID() const override { return m_id; }
    };

    class OpenGLFramebuffer : public FrameBuffer
    {
    public:
        // 拷贝
        static void CopyColor(const Ref<OpenGLFramebuffer> fr, uint32_t fr_idx, const Ref<OpenGLFramebuffer> to, uint32_t to_idx);
        static void CopyDepth(const Ref<OpenGLFramebuffer> fr, const Ref<OpenGLFramebuffer> to);
        static void CopyStencil(const Ref<OpenGLFramebuffer> fr, const Ref<OpenGLFramebuffer> to);

    public:
        OpenGLFramebuffer(uint32_t width, uint32_t height);
        virtual ~OpenGLFramebuffer();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual uint32_t GetRendererID() const override { return m_id; }
        virtual uint32_t GetWidth() const override { return m_width; }
        virtual uint32_t GetHeight() const override { return m_height; }

        // 颜色纹理
        virtual void AddColorTexture(uint32_t count, bool multisample = false) override;
        virtual void SetColorTexture(GLenum index, uint32_t texture_2d) override;
        virtual void SetColorTexture(GLenum index, uint32_t texture_cubemap, uint32_t face) override;
        virtual const Ref<Texture> GetColorTexture(GLenum index) const override;

        // 深度模板缓冲
        virtual void AddDepStTexture() override;
        virtual void AddDepthTexture3D() override;
        virtual void AddDepStRenderBuffer(bool multisample = false) override;
        virtual void AddDepthCubemap() override;
        virtual const Ref<Texture> GetDepthTexture() const override;
        virtual const uint32_t GetStencilTexView() const override;

        // 设置要绘制到的缓冲
        virtual void SetDrawBuffer(uint32_t index) const override;
        virtual void SetDrawBuffers(std::vector<uint32_t> indices) const override;
        virtual void SetDrawBuffers() const override;

        virtual void Draw(uint32_t index) const override;
        virtual void Clear(uint32_t index, glm::vec4 clear_color = {0.0f, 0.0f, 0.0f, 0.0f}) const override;
        virtual void Clear() const override;

    private:
        uint32_t m_width, m_height;
        // 完整性状态
        GLenum m_status = 0;

        // 颜色纹理
        std::vector<Ref<Texture>> m_color_attachments;

        // 深度模板缓冲
        Scope<RenderBuffer> m_depst_renderbuffer;
        Ref<Texture> m_depst_texture;
        uint32_t m_stencil_view;
    };

} // namespace Yutrel