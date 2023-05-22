#include "yutrel_pch.h"

#include "windows_window_system.h"

#include "runtime/function/global/global_context.h"

#include <GLFW/glfw3.h>

namespace Yutrel
{
    Windows_WindowSystem::~Windows_WindowSystem()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void Windows_WindowSystem::initialize(WindowCreateInfo create_info)
    {

        if (!glfwInit())
        {
            LOG_ERROR("failed to initialize GLFW");
            assert(false);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_width  = create_info.width;
        m_height = create_info.height;
        m_window = glfwCreateWindow(m_width, m_height, create_info.title, nullptr, nullptr);

        if (!m_window)
        {
            LOG_ERROR("failed to create window");
            assert(false);
        }

        glfwSetWindowUserPointer(m_window, this);

        // 设置回调函数
        glfwSetKeyCallback(m_window, keyCallback);
        glfwSetCharCallback(m_window, charCallback);
        glfwSetCharModsCallback(m_window, charModsCallback);
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
        glfwSetCursorPosCallback(m_window, cursorPosCallback);
        glfwSetCursorEnterCallback(m_window, cursorEnterCallback);
        glfwSetScrollCallback(m_window, scrollCallback);
        glfwSetDropCallback(m_window, dropCallback);
        glfwSetWindowSizeCallback(m_window, windowSizeCallback);
        glfwSetWindowCloseCallback(m_window, windowCloseCallback);
    }

    bool Windows_WindowSystem::shouldClose() const
    {
        return glfwWindowShouldClose(m_window);
    }

    std::array<int, 2> Windows_WindowSystem::getWindowSize() const
    {
        return std::array<int, 2>({m_width, m_height});
    }

    GLFWwindow *Windows_WindowSystem::getglfwWindow() const
    {
        return m_window;
    }

    void Windows_WindowSystem::pollEvents() const
    {
        glfwPollEvents();
    }

    void Windows_WindowSystem::setTitle(const char *title)
    {
        glfwSetWindowTitle(m_window, title);
    }

} // namespace Yutrel