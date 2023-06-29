#pragma once

#include "Yutrel/function/render/texture.hpp"

#include <memory>
#include <vector>

namespace Yutrel
{

    // 网格类
    class Mesh
    {
    public:
        virtual ~Mesh() = default;

        virtual void Draw() = 0;

        static Mesh* Create(std::vector<float>& vertices, std::vector<uint32_t>& indices, std::vector<Texture*>& textures);
    };
} // namespace Yutrel