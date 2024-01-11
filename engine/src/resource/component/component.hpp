#pragma once

#include "core/macro.hpp"

#include <glm/glm.hpp>

namespace Yutrel
{
    struct PbrBundle
    {
        Ref<struct Mesh> mesh;
        Ref<struct Material> material;
    };

    struct BackGroundColor
    {
        glm::vec4 data1;
        glm::vec4 data2;
        glm::vec4 data3;
        glm::vec4 data4;
    };

} // namespace Yutrel