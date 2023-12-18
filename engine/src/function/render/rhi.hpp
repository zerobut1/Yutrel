#pragma once

#include <stdint.h>

struct GLFWwindow;

namespace Yutrel
{
    struct RHIInitInfo
    {
        GLFWwindow* raw_window;
        uint32_t width;
        uint32_t height;
    };

    class RHI
    {
    public:
        virtual void Init(RHIInitInfo info) = 0;

        virtual void Clear() = 0;
    };
} // namespace Yutrel