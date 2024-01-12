#pragma once

namespace Yutrel
{
    // clang-format off
    static const std::vector<unsigned char> COMPUTE_CODE{
        #include "gradient.comp.spv.h"
    };

    static const std::vector<unsigned char> TRIANGLE_VERT_CODE{
        #include "triangle.vert.spv.h"
    };

    static const std::vector<unsigned char> TEXTURE_FRAG_CODE{
        #include "texture_image.frag.spv.h"
    };
} // namespace Yutrel