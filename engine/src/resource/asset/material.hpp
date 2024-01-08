#pragma once

#include "core/macro.hpp"

#include <glm/glm.hpp>

namespace Yutrel
{
    struct Texture;

    struct Material
    {
        glm::vec4 base_color;
        Ref<Texture> base_color_texture;
    };
} // namespace Yutrel