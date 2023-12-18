/**
 * 帧缓冲
 * 包含创建、添加颜色缓冲、添加深度模板缓冲等功能
 * 具体实现与平台有关
 */
#pragma once

#include "resource/asset/texture.hpp"

#include "glm/glm.hpp"

#include <initializer_list>
#include <memory>
#include <vector>

namespace Yutrel
{

    typedef unsigned int GLenum;

    class RenderBuffer : public Asset
    {
    public:
        // renderbuffer的所有权只归属framebuffer
        static Scope<RenderBuffer> Create(uint32_t width, uint32_t height, bool multisample = false);

        virtual ~RenderBuffer() = default;

        virtual void Bind() const   = 0;
        virtual void Unbind() const = 0;

        virtual uint32_t GetRendererID() const = 0;
    };

    class FrameBuffer : public Asset
    {
    public:
        static Ref<FrameBuffer> Create(uint32_t width, uint32_t height);
        // 拷贝
        static void CopyColor(const Ref<FrameBuffer> fr, uint32_t fr_idx, const Ref<FrameBuffer> to, uint32_t to_idx);
        static void CopyDepth(const Ref<FrameBuffer> fr, const Ref<FrameBuffer> to);
        static void CopyStencil(const Ref<FrameBuffer> fr, const Ref<FrameBuffer> to);

    public:
        virtual ~FrameBuffer() = default;

        virtual void Bind() const   = 0;
        virtual void Unbind() const = 0;

        virtual uint32_t GetRendererID() const = 0;
        virtual uint32_t GetWidth() const      = 0;
        virtual uint32_t GetHeight() const     = 0;

        // 颜色纹理
        virtual void AddColorTexture(uint32_t count, bool multisample = false)              = 0;
        virtual void SetColorTexture(GLenum index, uint32_t texture_2d)                     = 0;
        virtual void SetColorTexture(GLenum index, uint32_t texture_cubemap, uint32_t face) = 0;
        virtual const Ref<Texture> GetColorTexture(GLenum index) const                      = 0;

        // 深度模板缓冲
        virtual void AddDepStTexture()                              = 0;
        virtual void AddDepthTexture3D()                            = 0;
        virtual void AddDepStRenderBuffer(bool multisample = false) = 0;
        virtual void AddDepthCubemap()                              = 0;
        virtual const Ref<Texture> GetDepthTexture() const          = 0;
        virtual const uint32_t GetStencilTexView() const            = 0;

        // 设置要绘制到的缓冲
        virtual void SetDrawBuffer(uint32_t index) const                 = 0;
        virtual void SetDrawBuffers(std::vector<uint32_t> indices) const = 0;
        virtual void SetDrawBuffers() const                              = 0;

        virtual void Draw(uint32_t index) const                                                    = 0;
        virtual void Clear(uint32_t index, glm::vec4 clear_color = {0.0f, 0.0f, 0.0f, 0.0f}) const = 0;
        virtual void Clear() const                                                                 = 0;
    };

} // namespace Yutrel