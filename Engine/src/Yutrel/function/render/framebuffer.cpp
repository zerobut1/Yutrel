#include "framebuffer.h"
#include "yutrel_pch.hpp"

#include "Yutrel/platform/OpenGL/opengl_framebuffer.h"

namespace Yutrel
{
    std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification &spec)
    {
        return std::make_shared<OpenGLFramebuffer>(spec);
    }

} // namespace Yutrel
