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

    // struct SkyBox
    // {
    //     Ref<Texture> environment_map;
    //     Ref<Texture> irradiance_map;
    //     Ref<Texture> prefilter_map;

    //     float exposure = 1.0f;
    //     float lod      = 0.0f;

    //     SkyBox() = default;
    //     SkyBox(const std::string& path);
    // };

    // struct Scene
    // {};

    // struct

} // namespace Yutrel