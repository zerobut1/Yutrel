#pragma once

#include <Yutrel.hpp>
#include <stdint.h>

class ImguiUI : public Yutrel::WindowUI
{
public:
    virtual void RenderUI() override;

    static void UpdateData(gecs::resource<gecs::mut<Yutrel::UIResource>> ui,
                           gecs::resource<Yutrel::EngineStatus> status,
                           gecs::resource<Yutrel::Time> time);

public:
    Yutrel::EngineStatus status;
    uint32_t fps;
};