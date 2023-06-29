#include "yutrel_pch.hpp"

#include "windows_input.hpp"

#include "Yutrel/function/window/window.hpp"

#include <GLFW/glfw3.h>

namespace Yutrel
{
    bool WindowsInput::IsKeyPressed(const KeyCode key)
    {
        auto window_resource = (*Application::Get().GetWorld().GetResource<Window*>());
        GLFWwindow* window   = static_cast<GLFWwindow*>(window_resource->GetWindow());
        auto state           = glfwGetKey(window, static_cast<int32_t>(key));
        return state == GLFW_PRESS;
    }

    bool WindowsInput::IsMouseButtonPressed(const MouseCode button)
    {
        auto window_resource = (*Application::Get().GetWorld().GetResource<Window*>());
        GLFWwindow* window   = static_cast<GLFWwindow*>(window_resource->GetWindow());
        auto state           = glfwGetMouseButton(window, static_cast<int32_t>(button));
        return state == GLFW_PRESS;
    }

    glm::vec2 WindowsInput::GetMousePosition()
    {
        double xpos, ypos;
        auto window_resource = (*Application::Get().GetWorld().GetResource<Window*>());
        GLFWwindow* window   = static_cast<GLFWwindow*>(window_resource->GetWindow());
        glfwGetCursorPos(window, &xpos, &ypos);

        return {(float)xpos, (float)ypos};
    }

    float WindowsInput::GetMouseX()
    {
        return GetMousePosition().x;
    }

    float WindowsInput::GetMouseY()
    {
        return GetMousePosition().y;
    }
} // namespace Yutrel