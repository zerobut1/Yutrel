#pragma once

#include <vector>

namespace Yutrel
{
    // clang-format off
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