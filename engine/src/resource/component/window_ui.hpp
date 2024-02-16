#pragma once

#include "core/macro.hpp"

namespace Yutrel
{
    class WindowUI
    {
    public:
        virtual ~WindowUI() = default;

        virtual void Init() = 0;

        virtual void RenderUI() = 0;
    };

    class UIResource
    {
    public:
        UIResource() = default;
        UIResource(Ref<WindowUI> ui) : ui(ui) {}

        static void Init(gecs::resource<UIResource> ui_res)
        {
            ui_res->ui->Init();
        }

    public:
        Ref<WindowUI> ui;
    };
} // namespace Yutrel