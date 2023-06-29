#pragma once

#include "Yutrel/function/render/mesh.hpp"

#include <memory>
#include <string>
#include <vector>

namespace Yutrel
{
    // 模型类
    class Model
    {
    public:
        static Model* Create(std::string const& path);

        virtual ~Model() = default;

        virtual void Render() = 0;

    protected:
        std::vector<Mesh*> m_meshes;
        std::string m_directory;
    };
} // namespace Yutrel