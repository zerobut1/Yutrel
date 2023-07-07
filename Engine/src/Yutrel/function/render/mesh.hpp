#pragma once

#include "Yutrel/function/render/texture.hpp"
#include "Yutrel/function/render/vertex_array.hpp"

#include <memory>
#include <vector>

namespace Yutrel
{
    struct Material;

    // 网格类
    class Mesh
    {
        friend class Model;

    public:
        static Mesh* Create(std::vector<float>& vertices, std::vector<uint32_t>& indices, std::vector<Material*>& materials);

        Mesh(std::vector<float>& vertices, std::vector<uint32_t>& indices, std::vector<Material*>& materials);
        virtual ~Mesh() = default;

    private:
        std::vector<Material*> m_materials;

        VertexArray* m_VAO;
    };
} // namespace Yutrel