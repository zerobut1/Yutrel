#include "yutrel_pch.hpp"

#include "Yutrel/core/time.h"

#include "GLFW/glfw3.h"

namespace Yutrel
{
    float Time::getTime()
    {
        return glfwGetTime();
    }
} // namespace Yutrel