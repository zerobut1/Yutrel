#include "yutrel_pch.h"

#include "windows_input_system.h"

#include "runtime/function/global/global_context.h"

#include <GLFW/glfw3.h>

namespace Yutrel
{
    void WindowsInputSystem::initialize()
    {
        m_window = g_runtime_global_context.m_window_system->getglfwWindow();
    }

    bool WindowsInputSystem::IsKeyPressed(const KeyCode key)
    {
        auto state = glfwGetKey(m_window, static_cast<int32_t>(key));
        return state == GLFW_PRESS;
    }

    bool WindowsInputSystem::IsMouseButtonPressed(const MouseCode button)
    {
        auto state = glfwGetMouseButton(m_window, static_cast<int32_t>(button));
        return state == GLFW_PRESS;
    }

    glm::vec2 WindowsInputSystem::GetMousePosition()
    {
        double xpos, ypos;
        glfwGetCursorPos(m_window, &xpos, &ypos);

        return {(float)xpos, (float)ypos};
    }

    float WindowsInputSystem::GetMouseX()
    {
        return GetMousePosition().x;
    }

    float WindowsInputSystem::GetMouseY()
    {
        return GetMousePosition().y;
    }
} // namespace Yutrel