#include "yutrel_pch.hpp"

#include "framebuffer.hpp"

#include "Yutrel/platform/OpenGL/opengl_framebuffer.hpp"

namespace Yutrel
{
    Framebuffer* Framebuffer::Create(const FramebufferSpecification& spec)
    {
        return new OpenGLFramebuffer(spec);
    }

} // namespace Yutrel
