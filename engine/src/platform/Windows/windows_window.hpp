/**
 * windows平台下的窗口的实现
 */
#pragma once

#include "function/window/window.hpp"

#include "core/log/log.hpp"

#include <GLFW/glfw3.h>

#include <stdint.h>

namespace Yutrel
{
    class WindowsWindow : public Window
    {
    public:
        WindowsWindow(const std::string& title, uint32_t width, uint32_t height);
        virtual ~WindowsWindow();

        virtual void Tick() const override;
        virtual void SetTitle(const std::string& title) override;
        virtual bool ShouldClose() const override;
        virtual GLFWwindow* GetRaw() const override { return m_window; }
        virtual uint32_t Getwidth() const override { return m_width; };
        virtual uint32_t GetHeight() const override { return m_height; };

    private:
        GLFWwindow* m_window;
        uint32_t m_width;
        uint32_t m_height;

    private:
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->OnKey(key, scancode, action, mods);
            }
        }

        static void CharCallback(GLFWwindow* window, unsigned int codepoint)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->OnChar(codepoint);
            }
        }

        static void CharModsCallback(GLFWwindow* window, unsigned int codepoint, int mods)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->OnCharMods(codepoint, mods);
            }
        }

        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->OnMouseButton(button, action, mods);
            }
        }

        static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->OnCursorPos(xpos, ypos);
            }
        }

        static void CursorEnterCallback(GLFWwindow* window, int entered)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->OnCursorEnter(entered);
            }
        }

        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->OnScroll(xoffset, yoffset);
            }
        }

        static void DropCallback(GLFWwindow* window, int count, const char** paths)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->OnDrop(count, paths);
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