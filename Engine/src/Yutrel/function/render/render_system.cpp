#include "yutrel_pch.h"

#include "render_system.h"

#include "Yutrel/platform/OpenGL/opengl_render_system.h"

namespace Yutrel
{
    std::shared_ptr<RenderSystem> RenderSystem::Create()
    {
        // 暂时直接使用opengl
        return std::make_shared<OpenGLRenderSystem>();
    }
} // namespace Yutrel