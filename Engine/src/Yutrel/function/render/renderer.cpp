#include "yutrel_pch.hpp"

#include "renderer.hpp"

#include "Yutrel/platform/OpenGL/opengl_renderer.hpp"

namespace Yutrel
{
    Renderer* Renderer::Create(Window* window)
    {
        // 暂时直接使用opengl
        return new OpenGLRenderer(window);
    }
} // namespace Yutrel