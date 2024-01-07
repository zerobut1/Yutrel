#pragma once

#include "resource/asset/mesh.hpp"

namespace Yutrel
{
    struct PbrBundle
    {
        Ref<Mesh> mesh;
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