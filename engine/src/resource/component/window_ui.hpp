#pragma once

#include "core/macro.hpp"

namespace Yutrel
{
    class WindowUI
    {
    public:
        virtual ~WindowUI() = default;

        virtual void RenderUI() = 0;
    };

    class UIResource
    {
    public:
        UIResource() = default;
        UIResource(Ref<WindowUI> ui) : ui(ui) {}

    public:
        Ref<WindowUI> ui;
    };
} // namespace Yutrel