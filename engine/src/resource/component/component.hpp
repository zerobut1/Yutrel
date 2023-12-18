/**
 * 一些小组件
 */
#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "resource/asset/texture.hpp"
#include "resource/component/model/model.hpp"

namespace Yutrel
{
    struct SkyBox
    {
        Ref<Texture> environment_map;
        Ref<Texture> irradiance_map;
        Ref<Texture> prefilter_map;

        float exposure = 1.0f;
        float lod      = 0.0f;

        SkyBox() = default;
        SkyBox(const std::string& path);
    };

    struct Scene
    {};

} // namespace Yutrel