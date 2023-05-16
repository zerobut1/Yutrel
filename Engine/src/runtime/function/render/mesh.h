#pragma once

#include "runtime/function/render/texture.h"

#include <memory>
#include <vector>

namespace Yutrel
{
    class Mesh
    {
    public:
        virtual ~Mesh() = default;

        virtual void Draw() = 0;

        static std::shared_ptr<Mesh> create(std::vector<float> &vertices, std::vector<uint32_t> &indices, std::vector<std::shared_ptr<Texture>> &textures);
    };
} // namespace Yutrel