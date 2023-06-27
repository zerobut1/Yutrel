#include "yutrel_pch.h"

#include "windows_window.hpp"

#include "Yutrel/core/log/log.hpp"
#include "Yutrel/function/global/global_context.h"

#include <GLFW/glfw3.h>

namespace Yutrel
{

    WindowsWindow::WindowsWindow(std::string title, uint32_t width, uint32_t height)
    {
        if (!glfwInit())
        {
            LOG_ERROR("failed to initialize GLFW");
            assert(false);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_width  = width;
        m_height = height;
        m_window = glfwCreateWindow(m_width, m_height, title.c_str(), nullptr, nullptr);

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

    WindowsWindow::~WindowsWindow()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void WindowsWindow::tick() const
    {
        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }

    bool WindowsWindow::shouldClose() const
    {
        return glfwWindowShouldClose(m_window);
    }

    std::array<int, 2> WindowsWindow::getWindowSize() const
    {
        return std::array<int, 2>({m_width, m_height});
    }

    void* WindowsWindow::getWindow() const
    {
        return m_window;
    }

    void WindowsWindow::setTitle(const char* title)
    {
        glfwSetWindowTitle(m_window, title);
    }

} // namespace Yutrel