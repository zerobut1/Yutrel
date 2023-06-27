#pragma once

#include "Yutrel/function/window/window.hpp"

#include "Yutrel/core/application/application.hpp"

#include <GLFW/glfw3.h>

#include <iostream>

namespace Yutrel
{
    class WindowsWindow : public Window
    {
    public:
        WindowsWindow(std::string title, uint32_t width, uint32_t height);
        virtual ~WindowsWindow();

        virtual void tick() const override;

        virtual void setTitle(const char* title) override;
        virtual bool shouldClose() const override;
        virtual void* getWindow() const override;
        virtual std::array<int, 2> getWindowSize() const override;

    private:
        GLFWwindow* m_window;

    protected:
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->onKey(key, scancode, action, mods);
            }
        }

        static void charCallback(GLFWwindow* window, unsigned int codepoint)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->onChar(codepoint);
            }
        }

        static void charModsCallback(GLFWwindow* window, unsigned int codepoint, int mods)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->onCharMods(codepoint, mods);
            }
        }

        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->onMouseButton(button, action, mods);
            }
        }

        static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->onCursorPos(xpos, ypos);
            }
        }

        static void cursorEnterCallback(GLFWwindow* window, int entered)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->onCursorEnter(entered);
            }
        }

        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->onScroll(xoffset, yoffset);
            }
        }

        static void dropCallback(GLFWwindow* window, int count, const char** paths)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->onDrop(count, paths);
            }
        }

        static void windowSizeCallback(GLFWwindow* window, int width, int height)
        {
            WindowsWindow* windows_window = (WindowsWindow*)glfwGetWindowUserPointer(window);
            if (windows_window)
            {
                windows_window->m_width  = width;
                windows_window->m_height = height;
            }
        }

        static void windowCloseCallback(GLFWwindow* window)
        {
            glfwSetWindowShouldClose(window, true);
        }
    };
} // namespace Yutrel