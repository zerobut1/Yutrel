#pragma once

#include <Yutrel.hpp>
#include <stdint.h>

class ImguiUI : public Yutrel::WindowUI
{
public:
    virtual void RenderUI() override;

    static void UpdateData(gecs::resource<gecs::mut<Yutrel::UIResource>> ui,
                           gecs::resource<Yutrel::Time> time);

public:
    uint32_t fps;
};