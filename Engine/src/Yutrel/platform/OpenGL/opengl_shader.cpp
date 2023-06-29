#include "yutrel_pch.hpp"

#include "opengl_shader.hpp"

#include "Yutrel/core/log/log.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Yutrel
{
    OpenGLShader::OpenGLShader(const std::string& vertex_path, const std::string& fragment_path)
    {
        std::string vertexSrc   = readFile(vertex_path);
        std::string fragmentSrc = readFile(fragment_path);
        std::unordered_map<GLenum, std::string> sources;
        sources[GL_VERTEX_SHADER]   = vertexSrc;
        sources[GL_FRAGMENT_SHADER] = fragmentSrc;
        compile(sources);

        // 从path中取得文件名，作为shader的名字
        auto lastSlash = vertex_path.find_last_of("/\\");
        lastSlash      = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        auto lastDot   = vertex_path.rfind('.');
        auto count     = lastDot == std::string::npos ? vertex_path.size() - lastSlash : lastDot - lastSlash;
        m_Name         = vertex_path.substr(lastSlash, count);
    }

    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
        : m_Name(name)
    {
        std::unordered_map<GLenum, std::string> sources;
        sources[GL_VERTEX_SHADER]   = vertexSrc;
        sources[GL_FRAGMENT_SHADER] = fragmentSrc;
        compile(sources);
    }

    OpenGLShader::~OpenGLShader()
    {
        glDeleteProgram(m_RendererID);
    }

    std::string OpenGLShader::readFile(const std::string& filepath)
    {
        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary);
        if (in)
        {
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            if (size != -1)
            {
                result.resize(size);
                in.seekg(0, std::ios::beg);
                in.read(&result[0], size);
                in.close();
            }
            else
            {
                LOG_ERROR("Could not read from file '{0}", filepath);
            }
        }
        else
        {
            LOG_ERROR("Coule not open file '{0}'", filepath);
        }
        return result;
    }

    void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shaderSources)
    {

        GLuint program = glCreateProgram();

        assert(shaderSources.size() <= 2);

        std::array<GLenum, 2> glShaderIDs;
        int glShaderIDIndex = 0;
        for (auto& kv : shaderSources)
        {
            GLenum type               = kv.first;
            const std::string& source = kv.second;
            GLuint shader             = glCreateShader(type);

            // Send the vertex shader source code to GL
            // Note that std::string's .c_str is NULL character terminated.
            const GLchar* sourceCstr = source.c_str();
            glShaderSource(shader, 1, &sourceCstr, 0);

            // Compile the vertex shader
            glCompileShader(shader);

            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                // The maxLength includes the NULL character
                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

                // We don't need the shader anymore.
                glDeleteShader(shader);

                LOG_ERROR("{0}", infoLog.data());
                // ENGINE_CORE_ASSERT(false, "Vertex shader compilation failure!");
                break;
            }

            // Attach our shaders to our program
            glAttachShader(program, shader);
            glShaderIDs[glShaderIDIndex++] = shader;
        }

        m_RendererID = program;

        // Link our program
        glLinkProgram(program);
        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(program);

            for (auto id : glShaderIDs)
            {
                glDeleteShader(id);
            }

            LOG_ERROR("{0}", infoLog.data());
            // ENGINE_CORE_ASSERT(false, "Shader link failure!");
            return;
        }

        for (auto id : glShaderIDs)
        {
            glDetachShader(program, id);
            glDeleteShader(id);
        }
    }

    void OpenGLShader::Use() const
    {
        glUseProgram(m_RendererID);
    }

    void OpenGLShader::unUse() const
    {
        glUseProgram(0);
    }

    void OpenGLShader::setInt(const std::string& name, int value)
    {
        uploadUniformInt(name, value);
    }

    void OpenGLShader::setIntArray(const std::string& name, int* values, uint32_t count)
    {
        uploadUniformIntArray(name, values, count);
    }

    void OpenGLShader::setFloat(const std::string& name, float value)
    {
        uploadUniformFloat(name, value);
    }

    void OpenGLShader::setFloat2(const std::string& name, const glm::vec2& value)
    {
        uploadUniformFloat2(name, value);
    }

    void OpenGLShader::setFloat3(const std::string& name, const glm::vec3& value)
    {
        uploadUniformFloat3(name, value);
    }
    void OpenGLShader::setFloat4(const std::string& name, const glm::vec4& value)
    {
        uploadUniformFloat4(name, value);
    }

    void OpenGLShader::setMat4(const std::string& name, const glm::mat4& value)
    {
        uploadUniformMat4(name, value);
    }

    void OpenGLShader::uploadUniformInt(const std::string& name, int value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1i(location, value);
    }

    void OpenGLShader::uploadUniformIntArray(const std::string& name, int* values, uint32_t count)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1iv(location, count, values);
    }

    void OpenGLShader::uploadUniformFloat(const std::string& name, float value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1f(location, value);
    }

    void OpenGLShader::uploadUniformFloat2(const std::string& name, const glm::vec2& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform2f(location, value.x, value.y);
    }

    void OpenGLShader::uploadUniformFloat3(const std::string& name, const glm::vec3& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void OpenGLShader::uploadUniformFloat4(const std::string& name, const glm::vec4& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void OpenGLShader::uploadUniformMat3(const std::string& name, const glm::mat3& matrix)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void OpenGLShader::uploadUniformMat4(const std::string& name, const glm::mat4& matrix)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

} // namespace Yutrel