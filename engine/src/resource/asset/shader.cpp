#include "yutrel_pch.hpp"

#include "shader.hpp"

#include "platform/OpenGL/opengl_shader.hpp"

namespace Yutrel
{
    Ref<Shader> Shader::Create(const std::string& vertex_path, const std::string& fragment_path)
    {
        return CreateRef<OpenGLShader>(vertex_path, fragment_path);
    }

    Ref<Shader> Shader::Create(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path)
    {
        return CreateRef<OpenGLShader>(vertex_path, fragment_path, geometry_path);
    }

    Ref<ComputeShader> ComputeShader::Create(const std::string& shader_path)
    {
        return CreateRef<OpenGLComputeShader>(shader_path);
    }

} // namespace Yutrel