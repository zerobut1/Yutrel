#pragma once

#include "Yutrel/function/window/window.hpp"

#include "Yutrel/core/macro.hpp"

#include <GLFW/glfw3.h>

namespace Yutrel
{
    /*
     * windows平台下的窗口
     */
    class WindowsWindow : public Window
    {
    public:
        WindowsWindow(std::string title, uint32_t width, uint32_t height);
        virtual ~WindowsWindow();

        virtual void Tick() const override;
        virtual void SetTitle(const char* title) override;
        virtual bool ShouldClose() const override;
        virtual void* GetWindow() const override;
        virtual std::pair<float, float> GetWindowSize() const override;

    private:
        GLFWwindow* m_window;
        float m_width;
        float m_height;

    private:
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->onKey(key, scancode, action, mods);
            }
        }

        static void CharCallback(GLFWwindow* window, unsigned int codepoint)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->onChar(codepoint);
            }
        }

        static void CharModsCallback(GLFWwindow* window, unsigned int codepoint, int mods)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->onCharMods(codepoint, mods);
            }
        }

        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->onMouseButton(button, action, mods);
            }
        }

        static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->onCursorPos(xpos, ypos);
            }
        }

        static void CursorEnterCallback(GLFWwindow* window, int entered)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->onCursorEnter(entered);
            }
        }

        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->onScroll(xoffset, yoffset);
            }
        }

        static void DropCallback(GLFWwindow* window, int count, const char** paths)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->onDrop(count, paths);
            }
        }

        static void WindowSizeCallback(GLFWwindow* window, int width, int height)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->m_width  = width;
                windows_window->m_height = height;
            }
        }

        static void WindowCloseCallback(GLFWwindow* window)
        {
            glfwSetWindowShouldClose(window, true);
        }

        static void GLFWErrorCallback(int error, const char* description)
        {
            LOG_ERROR("GLFW Error ({0}): {1}", error, description);
        }
    };
} // namespace Yutrel