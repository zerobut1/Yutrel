#pragma once

#include "Yutrel/function/input/input.hpp"

#include "GLFW/glfw3.h"

namespace Yutrel
{
    class WindowsInput : public Input
    {
    public:
        virtual bool IsKeyPressed(KeyCode key) override;
        virtual bool IsMouseButtonPressed(MouseCode button) override;
        virtual glm::vec2 GetMousePosition() override;
        virtual float GetMouseX() override;
        virtual float GetMouseY() override;
    };
} // namespace Yutrel