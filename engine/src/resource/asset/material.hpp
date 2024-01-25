#pragma once

#include "core/macro.hpp"

#include <glm/glm.hpp>

namespace Yutrel
{
    struct Texture;

    struct Material
    {
        glm::vec4 base_color_factor;
        float metallic_factor;
        float roughness_factor;
        Ref<Texture> base_color_texture;
        Ref<Texture> metallic_roughness_texture;
        Ref<Texture> normal_texture;
    };
} // namespace Yutrel