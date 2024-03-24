#pragma once

#include <vector>

namespace Yutrel
{
    // clang-format off
    static const std::vector<unsigned char> BASE_PASS_VERT_CODE{
        #include "basepass.vert.spv.h"
    };

    static const std::vector<unsigned char> BASE_PASS_FRAG_CODE{
        #include "basepass.frag.spv.h"
    };

    static const std::vector<unsigned char> LIGHTING_VERT_CODE{
        #include "lighting.vert.spv.h"
    };

    static const std::vector<unsigned char> LIGHTING_FRAG_CODE{
        #include "lighting.frag.spv.h"
    };

    static const std::vector<unsigned char> WATER_VERT_CODE{
        #include "water.vert.spv.h"
    };

    static const std::vector<unsigned char> WATER_FRAG_CODE{
        #include "water.frag.spv.h"
    };

    static const std::vector<unsigned char> PBR_VERT_CODE{
        #include "pbr.vert.spv.h"
    };

    static const std::vector<unsigned char> PBR_FRAG_CODE{
        #include "pbr.frag.spv.h"
    };

    static const std::vector<unsigned char> SHADOWMAP_VERT_CODE{
        #include "shadowmap.vert.spv.h"
    };

    static const std::vector<unsigned char> SHADOWMAP_FRAG_CODE{
        #include "shadowmap.frag.spv.h"
    };
    
    static const std::vector<unsigned char> QUAD_VERT_CODE{
        #include "quad.vert.spv.h"
    };

    static const std::vector<unsigned char> QUAD_FRAG_CODE{
        #include "quad.frag.spv.h"
    };

    static const std::vector<unsigned char> SKYBOX_VERT_CODE{
        #include "skybox.vert.spv.h"
    };

    static const std::vector<unsigned char> SKYBOX_FRAG_CODE{
        #include "skybox.frag.spv.h"
    };

} // namespace Yutrel