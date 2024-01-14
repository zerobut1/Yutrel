#pragma once

#include <string>
namespace Yutrel
{
    struct GLTFScene
    {
    public:
        GLTFScene() = default;
        GLTFScene(const std::string& path)
            : path(path) {}

    public:
        std::string path;
        bool is_loaded{false};
    };
} // namespace Yutrel