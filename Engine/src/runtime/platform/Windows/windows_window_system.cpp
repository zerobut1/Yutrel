#include "windows_window_system.h"

#include <GLFW/glfw3.h>

namespace Yutrel
{
    Windows_WindowSystem::~Windows_WindowSystem()
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Windows_WindowSystem::initialize()
    {
        // 目前只简单初始化了glfw，然后创建了窗口

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_Window = glfwCreateWindow(1920, 1080, "test", nullptr, nullptr);
    }

    bool Windows_WindowSystem::shouldClose() const
    {
        return glfwWindowShouldClose(m_Window);
    }

} // namespace Yutrel