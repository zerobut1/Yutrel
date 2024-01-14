#pragma once

#include <Yutrel.hpp>
#include <stdint.h>

class ImguiUI : public Yutrel::WindowUI
{
public:
    virtual void RenderUI() override;

    static void UpdateData(gecs::resource<gecs::mut<Yutrel::UIResource>> ui,
                           gecs::resource<Yutrel::EngineStatus> status,
                           gecs::resource<Yutrel::Time> time,
                           gecs::resource<gecs::mut<Yutrel::BackGroundColor>> background_color);

public:
    Yutrel::BackGroundColor background{
        {0.2f, 0.2f, 0.2f, 0.0f},
        {0.2f, 0.2f, 0.2f, 0.0f},
        {0.2f, 0.2f, 0.2f, 0.0f},
        {0.2f, 0.2f, 0.2f, 0.0f},
    };

    Yutrel::EngineStatus status;
    uint32_t fps;
};