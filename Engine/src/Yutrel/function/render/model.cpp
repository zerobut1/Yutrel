#include "yutrel_pch.hpp"

#include "model.h"

#include "Yutrel/platform/OpenGL/opengl_model.h"

namespace Yutrel
{
    std::shared_ptr<Model> Model::Create(std::string const &path)
    {
        return std::make_shared<OpenGLModel>(path);
    }
} // namespace Yutrel