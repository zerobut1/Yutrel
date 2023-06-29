#include "yutrel_pch.hpp"

#include "shader.hpp"

#include "Yutrel/platform/OpenGL/opengl_shader.hpp"

namespace Yutrel
{
    Shader* Shader::Create(const std::string& vertex_path, const std::string& fragment_path)
    {
        return new OpenGLShader(vertex_path, fragment_path);
    }

    Shader* Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        return new OpenGLShader(name, vertexSrc, fragmentSrc);
    }
} // namespace Yutrel