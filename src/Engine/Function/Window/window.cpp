#include "window.h"

#include <Core/log.h>

namespace Yutrel
{
    Window::Window(const WindowCreateInfo& info)
    {
        LOG_INFO("Creating Window : {0} ({1},{2})", info.title, info.width, info.height);
        init(info);
    }

    Window::~Window()
    {
        shutdown();
    }

    void Window::init(const WindowCreateInfo& info)
    {
        // 初始化glfw
        if (!glfwInit())
        {
            LOG_CRITICAL("Failed to Init GLFW");
            return;
        }

        // Vulkan则禁止创建opengl上下文
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        // 创建窗口
        m_width  = info.width;
        m_height = info.height;
        m_window = glfwCreateWindow(m_width, m_height, info.title.c_str(), nullptr, nullptr);
        if (!m_window)
        {
            LOG_CRITICAL("Failed to Create GLFW Window");
            glfwTerminate();
            return;
        }

        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        glfwSetWindowUserPointer(m_window, this);

        glfwSetErrorCallback(errorCallback);

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

    void Window::shutdown()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void Window::pollEvents() const
    {
        glfwPollEvents();
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(m_window);
    }

    void Window::setTitle(const std::string& title)
    {
        glfwSetWindowTitle(m_window, title.c_str());
    }

    void Window::errorCallback(int error, const char* description)
    {
        LOG_ERROR("GLFW ERROR : {}", description);
    }

    void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            _window->onKey(key, scancode, action, mods);
        }
    }

    void Window::charCallback(GLFWwindow* window, unsigned int codepoint)
    {
        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            _window->onChar(codepoint);
        }
    }

    void Window::charModsCallback(GLFWwindow* window, unsigned int codepoint, int mods)
    {
        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            _window->onCharMods(codepoint, mods);
        }
    }

    void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            _window->onMouseButton(button, action, mods);
        }
    }

    void Window::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            _window->onCursorPos(xpos, ypos);
        }
    }

    void Window::cursorEnterCallback(GLFWwindow* window, int entered)
    {
        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            _window->onCursorEnter(entered);
        }
    }

    void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            _window->onScroll(xoffset, yoffset);
        }
    }

    void Window::dropCallback(GLFWwindow* window, int count, const char** paths)
    {
        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            _window->onDrop(count, paths);
        }
    }

    void Window::windowSizeCallback(GLFWwindow* window, int width, int height)
    {
        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            _window->onWindowSize(width, height);
        }
    }

    void Window::windowCloseCallback(GLFWwindow* window)
    {
        glfwSetWindowShouldClose(window, true);
    }

    void Window::onKey(int key, int scancode, int action, int mods)
    {
        for (auto& func : m_OnKeyFunc)
            func(key, scancode, action, mods);
    }
    void Window::onChar(unsigned int codepoint)
    {
        for (auto& func : m_OnCharFunc)
            func(codepoint);
    }
    void Window::onCharMods(int codepoint, unsigned int mods)
    {
        for (auto& func : m_OnCharModsFunc)
            func(codepoint, mods);
    }
    void Window::onMouseButton(int button, int action, int mods)
    {
        for (auto& func : m_OnMouseButtonFunc)
            func(button, action, mods);
    }
    void Window::onCursorPos(double xpos, double ypos)
    {
        for (auto& func : m_OnCursorPosFunc)
            func(xpos, ypos);
    }
    void Window::onCursorEnter(int entered)
    {
        for (auto& func : m_OnCursorEnterFunc)
            func(entered);
    }
    void Window::onScroll(double xoffset, double yoffset)
    {
        for (auto& func : m_OnScrollFunc)
            func(xoffset, yoffset);
    }
    void Window::onDrop(int count, const char** paths)
    {
        for (auto& func : m_OnDropFunc)
            func(count, paths);
    }
    void Window::onWindowSize(int width, int height)
    {
        for (auto& func : m_OnWindowSizeFunc)
            func(width, height);
    }

} // namespace Yutrel