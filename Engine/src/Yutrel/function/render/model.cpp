#include "yutrel_pch.hpp"

#include "model.hpp"

#include "Yutrel/platform/OpenGL/opengl_model.hpp"

namespace Yutrel
{
    Model* Model::Create(std::string const& path)
    {
        return new OpenGLModel(path);
    }
} // namespace Yutrel