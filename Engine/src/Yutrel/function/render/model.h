#pragma once

#include "Yutrel/function/render/mesh.h"

#include <memory>
#include <string>
#include <vector>

namespace Yutrel
{
    // 模型类
    class Model
    {
    public:
        static std::shared_ptr<Model> Create(std::string const &path);

        virtual ~Model() = default;

        virtual void Render() = 0;

    protected:
        std::vector<std::shared_ptr<Mesh>> m_meshes;
        std::string m_directory;
    };
} // namespace Yutrel