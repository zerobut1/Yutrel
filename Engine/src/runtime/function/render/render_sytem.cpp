#include "render_system.h"

#include "runtime/platform/OpenGL/opengl_render_system.h"

#include <memory>

namespace Yutrel
{
    std::shared_ptr<RenderSystem> RenderSystem::create()
    {
        //暂时直接使用opengl
        return std::make_shared<OpenGLRenderSystem>();
    }
} // namespace Yutrel