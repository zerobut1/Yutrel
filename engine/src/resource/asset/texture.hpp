#pragma once

#include <string>

namespace Yutrel
{
    struct Texture
    {
        std::string path;
        bool is_loaded{false};
        bool is_uploaded{false};
    };
} // namespace Yutrel