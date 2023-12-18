#pragma once

struct GLFWwindow;

namespace Yutrel
{
    struct RHIInitInfo
    {
        GLFWwindow* raw_window;
    };

    class RHI
    {
    public:
        virtual void Init(RHIInitInfo info) = 0;

        virtual void Clear() = 0;
    };
} // namespace Yutrel