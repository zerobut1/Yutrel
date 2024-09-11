#include "window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <Core/log.h>

namespace Yutrel
{
    Window::Window(const CreateInfo& info)
        : m_callbacks(info.callbacks)
    {
        LOG_INFO("Creating Window : {0} ({1},{2})", info.title, info.width, info.height);
        init(info);
    }

    Window::~Window()
    {
        shutdown();
    }

    void Window::init(const CreateInfo& info)
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
        m_width       = info.width;
        m_height      = info.height;
        m_GLFW_window = glfwCreateWindow(m_width, m_height, info.title.c_str(), nullptr, nullptr);
        if (!m_GLFW_window)
        {
            LOG_CRITICAL("Failed to Create GLFW Window");
            glfwTerminate();
            return;
        }

        glfwSetInputMode(m_GLFW_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        glfwSetWindowUserPointer(m_GLFW_window, this);

        glfwSetErrorCallback(errorCallback);

        // 设置回调函数
        glfwSetKeyCallback(m_GLFW_window, keyCallback);
        glfwSetCharCallback(m_GLFW_window, charCallback);
        glfwSetCharModsCallback(m_GLFW_window, charModsCallback);
        glfwSetMouseButtonCallback(m_GLFW_window, mouseButtonCallback);
        glfwSetCursorPosCallback(m_GLFW_window, cursorPosCallback);
        glfwSetCursorEnterCallback(m_GLFW_window, cursorEnterCallback);
        glfwSetScrollCallback(m_GLFW_window, scrollCallback);
        glfwSetDropCallback(m_GLFW_window, dropCallback);
        glfwSetWindowSizeCallback(m_GLFW_window, windowSizeCallback);
        glfwSetWindowCloseCallback(m_GLFW_window, windowCloseCallback);
    }

    void Window::shutdown()
    {
        glfwDestroyWindow(m_GLFW_window);
        glfwTerminate();
    }

    void Window::pollEvents() const
    {
        glfwPollEvents();
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(m_GLFW_window);
    }

    void Window::setTitle(const std::string& title)
    {
        glfwSetWindowTitle(m_GLFW_window, title.c_str());
    }

    void Window::resize(uint32_t width, uint32_t height)
    {
        glfwSetWindowSize(m_GLFW_window, width, height);
        updateWindowSize();
        m_callbacks->handleWindowSizeChange();
    }

    void Window::updateWindowSize()
    {
        int32_t width, height;
        glfwGetWindowSize(m_GLFW_window, &width, &height);
        setWindowSize(width, height);
    }

    void Window::setWindowSize(uint32_t width, uint32_t height)
    {
        YUTREL_ASSERT(width > 0 && height > 0, "");

        m_width  = width;
        m_height = height;
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
            // _window->onKey(key, scancode, action, mods);
        }
    }

    void Window::charCallback(GLFWwindow* window, unsigned int codepoint)
    {
        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            // _window->onChar(codepoint);
        }
    }

    void Window::charModsCallback(GLFWwindow* window, unsigned int codepoint, int mods)
    {
        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            // _window->onCharMods(codepoint, mods);
        }
    }

    void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            // _window->onMouseButton(button, action, mods);
        }
    }

    void Window::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            // _window->onCursorPos(xpos, ypos);
        }
    }

    void Window::cursorEnterCallback(GLFWwindow* window, int entered)
    {
        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            // _window->onCursorEnter(entered);
        }
    }

    void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            // _window->onScroll(xoffset, yoffset);
        }
    }

    void Window::dropCallback(GLFWwindow* window, int count, const char** paths)
    {
        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            // _window->onDrop(count, paths);
        }
    }

    void Window::windowSizeCallback(GLFWwindow* window, int width, int height)
    {
        if (width == 0 || height == 0)
        {
            return;
        }

        auto _window = (Window*)glfwGetWindowUserPointer(window);
        if (_window)
        {
            _window->resize(width, height);
        }
    }

    void Window::windowCloseCallback(GLFWwindow* window)
    {
        glfwSetWindowShouldClose(window, true);
    }

} // namespace Yutrel