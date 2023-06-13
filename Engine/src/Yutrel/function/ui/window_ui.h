#pragma once

#include <array>
#include <memory>
#include <stdint.h>

namespace Yutrel
{
    class WindowUI
    {
    public:
        virtual void initialize() = 0;
        virtual void preRender()  = 0;
        virtual void clear()      = 0;
    };
} // namespace Yutrel