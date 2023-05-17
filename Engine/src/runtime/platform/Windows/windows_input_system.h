#pragma once

#include "GLFW/glfw3.h"
#include "runtime/function/input/input_system.h"

namespace Yutrel
{
    class WindowsInputSystem : public InputSystem
    {
    public:
        virtual void initialize() override;

        virtual bool IsKeyPressed(KeyCode key) override;
        virtual bool IsMouseButtonPressed(MouseCode button) override;
        virtual glm::vec2 GetMousePosition() override;
        virtual float GetMouseX() override;
        virtual float GetMouseY() override;

    private:
        GLFWwindow *m_window;
    };
} // namespace Yutrel