#pragma once

#include "Yutrel/core/inputcode/key_codes.h"
#include "Yutrel/core/inputcode/mouse_codes.h"

// 暂且使用glm
#include <glm/glm.hpp>

#include <memory>

namespace Yutrel
{
    class InputSystem
    {
    public:
        static std::shared_ptr<InputSystem> Create();

        virtual void initialize()                           = 0;
        virtual bool IsKeyPressed(KeyCode key)              = 0;
        virtual bool IsMouseButtonPressed(MouseCode button) = 0;
        virtual glm::vec2 GetMousePosition()                = 0;
        virtual float GetMouseX()                           = 0;
        virtual float GetMouseY()                           = 0;
    };
} // namespace Yutrel