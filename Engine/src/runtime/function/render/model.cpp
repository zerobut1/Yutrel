#include "model.h"

#include "runtime/platform/OpenGL/opengl_model.h"

#include <memory>

namespace Yutrel
{
    std::shared_ptr<Model> Model::create(std::string const &path)
    {
        return std::make_shared<OpenGLModel>(path);
    }
} // namespace Yutrel