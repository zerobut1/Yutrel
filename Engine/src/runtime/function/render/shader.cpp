#include "shader.h"

#include "runtime/platform/OpenGL/opengl_shader.h"
#include <memory>

namespace Yutrel
{
    std::shared_ptr<Shader> Shader::create(const std::string &vertex_path, const std::string &fragment_path)
    {
        return std::make_shared<OpenGLShader>(vertex_path, fragment_path);
    }

    std::shared_ptr<Shader> Shader::create(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc)
    {
        return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
    }
} // namespace Yutrel