#pragma once

#include "Yutrel/function/render/model.hpp"
#include "Yutrel/function/render/texture.hpp"

namespace Yutrel
{
    struct SkyBox
    {
        TextureCubemaps* texture;
    };

} // namespace Yutrel