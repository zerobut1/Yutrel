#include "windows_window_system.h"

#include <GLFW/glfw3.h>

#include <array>

namespace Yutrel
{
    Windows_WindowSystem::~Windows_WindowSystem()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void Windows_WindowSystem::initialize()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_width = 1920;
        m_height = 1080;
        m_window = glfwCreateWindow(1920, 1080, "test", nullptr, nullptr);

        glfwSetWindowUserPointer(m_window, this);

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

} // namespace Yutrel