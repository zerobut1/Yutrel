#pragma once

#include <array>
#include <memory>
#include <stdint.h>

namespace Yutrel
{
    class WindowUI
    {
    public:
        virtual void RenderUI() = 0;
    };
} // namespace Yutrel