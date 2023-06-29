#pragma once

#include "inputcode/key_codes.h"
#include "inputcode/mouse_codes.h"

// 暂且使用glm
#include <glm/glm.hpp>

#include <memory>

namespace Yutrel
{
    class Input
    {
    public:
        static Input* Create();

        virtual bool IsKeyPressed(KeyCode key)              = 0;
        virtual bool IsMouseButtonPressed(MouseCode button) = 0;
        virtual glm::vec2 GetMousePosition()                = 0;
        virtual float GetMouseX()                           = 0;
        virtual float GetMouseY()                           = 0;
    };
} // namespace Yutrel
