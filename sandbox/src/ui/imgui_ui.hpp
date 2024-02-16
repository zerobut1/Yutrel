#pragma once

#include <Yutrel.hpp>

#include <stdint.h>

class ImguiUI : public Yutrel::WindowUI
{
public:
    virtual void RenderUI() override;

    virtual void Init() override;

    static void UpdateData(gecs::resource<gecs::mut<Yutrel::UIResource>> ui,
                           gecs::querier<gecs::mut<Yutrel::DirectionalLight>> directional_lights,
                           gecs::resource<Yutrel::Time> time);

public:
    uint32_t fps;

    glm::vec3 directional_light_direction{};
};