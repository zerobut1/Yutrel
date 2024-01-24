#pragma once

#include <gecs/gecs.hpp>

#include <string>
#include <vector>

namespace Yutrel
{
    struct Primitives
    {
        std::vector<gecs::entity> entities;
    };

    struct GLTFScene
    {
    public:
        GLTFScene() = default;
        GLTFScene(const std::string& path)
            : path(path) {}

    public:
        std::string path;
        std::string root_path;
        bool is_loaded{false};
    };
} // namespace Yutrel