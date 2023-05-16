#pragma once

#include "runtime/function/render/mesh.h"

#include <memory>
#include <string>
#include <vector>

namespace Yutrel
{
    class Model
    {
    public:
        virtual ~Model() = default;

        virtual void Draw() = 0;

        static std::shared_ptr<Model> create(std::string const &path);

    protected:
        std::vector<std::shared_ptr<Mesh>> meshes;
        std::string directory;
    };
} // namespace Yutrel