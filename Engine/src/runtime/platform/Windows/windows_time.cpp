#pragma once

#include "runtime/core/time.h"

#include "GLFW/glfw3.h"

namespace Yutrel
{
    float Time::getTime()
    {
        return glfwGetTime();
    }
} // namespace Yutrel