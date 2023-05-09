#pragma once

#include "runtime/function/window/window_system.h"

#include <GLFW/glfw3.h>

namespace Yutrel
{
    class Windows_WindowSystem : public WindowSystem
    {
    public:
        virtual ~Windows_WindowSystem();

        virtual void initialize(WindowCreateInfo create_info) override;
        virtual void pollEvents() const override;
        virtual void setTitle(const char *title) override;
        virtual bool shouldClose() const override;
        virtual GLFWwindow *getglfwWindow() const override;
        virtual std::array<int, 2> getWindowSize() const override;

    protected:
        static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
        {
            Windows_WindowSystem *app = (Windows_WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onKey(key, scancode, action, mods);
            }
        }

        static void charCallback(GLFWwindow *window, unsigned int codepoint)
        {
            Windows_WindowSystem *app = (Windows_WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onChar(codepoint);
            }
        }

        static void charModsCallback(GLFWwindow *window, unsigned int codepoint, int mods)
        {
            Windows_WindowSystem *app = (Windows_WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onCharMods(codepoint, mods);
            }
        }

        static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
        {
            Windows_WindowSystem *app = (Windows_WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onMouseButton(button, action, mods);
            }
        }

        static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
        {
            Windows_WindowSystem *app = (Windows_WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onCursorPos(xpos, ypos);
            }
        }

        static void cursorEnterCallback(GLFWwindow *window, int entered)
        {
            Windows_WindowSystem *app = (Windows_WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onCursorEnter(entered);
            }
        }

        static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
        {
            Windows_WindowSystem *app = (Windows_WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onScroll(xoffset, yoffset);
            }
        }

        static void dropCallback(GLFWwindow *window, int count, const char **paths)
        {
            Windows_WindowSystem *app = (Windows_WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->onDrop(count, paths);
            }
        }

        static void windowSizeCallback(GLFWwindow *window, int width, int height)
        {
            Windows_WindowSystem *app = (Windows_WindowSystem *)glfwGetWindowUserPointer(window);
            if (app)
            {
                app->m_width  = width;
                app->m_height = height;
            }
        }

        static void windowCloseCallback(GLFWwindow *window)
        {
            glfwSetWindowShouldClose(window, true);
        }

    private:
        GLFWwindow *m_window;
    };
} // namespace Yutrel