#pragma once

#include <Yutrel.hpp>

class ImguiUI : public Yutrel::WindowUI
{
public:
    virtual void RenderUI() override;

    static void UpdateData(gecs::resource<Yutrel::UIResource> ui,
                           gecs::resource<gecs::mut<Yutrel::BackGroundColor>> background_color);

public:
    Yutrel::BackGroundColor background{
        {0.2f, 0.2f, 0.2f, 0.0f},
        {0.2f, 0.2f, 0.2f, 0.0f},
        {0.2f, 0.2f, 0.2f, 0.0f},
        {0.2f, 0.2f, 0.2f, 0.0f},
    };
};