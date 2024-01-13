#include "yutrel_pch.hpp"

#include "windows_window.hpp"

#include <GLFW/glfw3.h>
#include <stdint.h>

namespace Yutrel
{
    WindowsWindow::WindowsWindow(const std::string& title, uint32_t width, uint32_t height)
    {
        LOG_INFO("Creating Window : {0} ({1},{2})", title, width, height);

        {
            int success = glfwInit();
            YUTREL_ASSERT(success, "Could not initialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
        }

        /*
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        */

        // Vulkan则禁止创建opengl上下文
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        // 创建窗口
        m_width  = width;
        m_height = height;
        m_window = glfwCreateWindow(m_width, m_height, title.c_str(), nullptr, nullptr);
        YUTREL_ASSERT(m_window, "Failed to create window!");

        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        glfwSetWindowUserPointer(m_window, this);

        // 设置回调函数
        glfwSetKeyCallback(m_window, KeyCallback);
        glfwSetCharCallback(m_window, CharCallback);
        glfwSetCharModsCallback(m_window, CharModsCallback);
        glfwSetMouseButtonCallback(m_window, MouseButtonCallback);
        glfwSetCursorPosCallback(m_window, CursorPosCallback);
        glfwSetCursorEnterCallback(m_window, CursorEnterCallback);
        glfwSetScrollCallback(m_window, ScrollCallback);
        glfwSetDropCallback(m_window, DropCallback);
        glfwSetWindowSizeCallback(m_window, WindowSizeCallback);
        glfwSetWindowCloseCallback(m_window, WindowCloseCallback);
    }

    WindowsWindow::~WindowsWindow()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void WindowsWindow::Tick() const
    {
        glfwPollEvents();
        // glfwSwapBuffers(m_window);
    }

    bool WindowsWindow::ShouldClose() const
    {
        return glfwWindowShouldClose(m_window);
    }

    void WindowsWindow::SetTitle(const std::string& title)
    {
        glfwSetWindowTitle(m_window, title.c_str());
    }

} // namespace Yutrel