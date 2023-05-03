#pragma once

#include "runtime/function/window/window_system.h"

#include <GLFW/glfw3.h>

namespace Yutrel
{
    class Windows_WindowSystem : public WindowSystem
    {
    public:
        virtual ~Windows_WindowSystem();
        virtual void initialize() override;

        virtual bool shouldClose() const override;

        virtual GLFWwindow *getglfwWindow() const override { return m_Window; }

        virtual void pollEvents() const override { glfwPollEvents(); }

    protected:
        static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
        {
            WindowSystem *app = (WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onKey(key, scancode, action, mods);
            }
        }
        static void charCallback(GLFWwindow *window, unsigned int codepoint)
        {
            WindowSystem *app = (WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onChar(codepoint);
            }
        }
        static void charModsCallback(GLFWwindow *window, unsigned int codepoint, int mods)
        {
            WindowSystem *app = (WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onCharMods(codepoint, mods);
            }
        }
        static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
        {
            WindowSystem *app = (WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onMouseButton(button, action, mods);
            }
        }
        static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
        {
            WindowSystem *app = (WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onCursorPos(xpos, ypos);
            }
        }
        static void cursorEnterCallback(GLFWwindow *window, int entered)
        {
            WindowSystem *app = (WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onCursorEnter(entered);
            }
        }
        static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
        {
            WindowSystem *app = (WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onScroll(xoffset, yoffset);
            }
        }
        static void dropCallback(GLFWwindow *window, int count, const char **paths)
        {
            WindowSystem *app = (WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onDrop(count, paths);
            }
        }
        static void windowSizeCallback(GLFWwindow *window, int width, int height)
        {
            WindowSystem *app = (WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->m_width  = width;
                app->m_height = height;
            }
        }
        static void windowCloseCallback(GLFWwindow *window) { glfwSetWindowShouldClose(window, true); }

    private:
        GLFWwindow *m_Window;
    };
} // namespace Yutrel