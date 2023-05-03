#pragma once

#include "GLFW/glfw3.h"
#include "runtime/function/render/render_system.h"

namespace Yutrel
{
    class OpenGL_RenderSystem : public RenderSystem
    {
    public:
        OpenGL_RenderSystem() = default;
        virtual ~OpenGL_RenderSystem();

        virtual void initialize(RenderSystemInitInfo render_init_info) override;

        virtual void tick() override;


    private:
        GLFWwindow* m_window;
        unsigned int shaderProgram;
        unsigned int VAO, VBO;
    };
} // namespace Yutrel