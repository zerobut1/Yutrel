#include "yutrel_pch.hpp"

#include "framebuffer.hpp"

#include "platform/OpenGL/opengl_framebuffer.hpp"
#include <memory>

namespace Yutrel
{
    Scope<RenderBuffer> RenderBuffer::Create(uint32_t width, uint32_t height, bool multisample)
    {
        return CreateScope<OpenGLRenderBuffer>(width, height, multisample);
    }

    Ref<FrameBuffer> FrameBuffer::Create(uint32_t width, uint32_t height)
    {
        return CreateRef<OpenGLFramebuffer>(width, height);
    }

    void FrameBuffer::CopyColor(const Ref<FrameBuffer> fr, uint32_t fr_idx, const Ref<FrameBuffer> to, uint32_t to_idx)
    {
        OpenGLFramebuffer::CopyColor(std::static_pointer_cast<OpenGLFramebuffer>(fr), fr_idx, std::static_pointer_cast<OpenGLFramebuffer>(to), to_idx);
    }

    void FrameBuffer::CopyDepth(const Ref<FrameBuffer> fr, const Ref<FrameBuffer> to)
    {
        OpenGLFramebuffer::CopyDepth(std::static_pointer_cast<OpenGLFramebuffer>(fr), std::static_pointer_cast<OpenGLFramebuffer>(to));
    }

    void FrameBuffer::CopyStencil(const Ref<FrameBuffer> fr, const Ref<FrameBuffer> to)
    {
        OpenGLFramebuffer::CopyStencil(std::static_pointer_cast<OpenGLFramebuffer>(fr), std::static_pointer_cast<OpenGLFramebuffer>(to));
    }
} // namespace Yutrel