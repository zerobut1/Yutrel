#pragma once

#include "runtime/function/render/shader.h"
#include <unordered_map>

typedef unsigned int GLenum;

namespace Yutrel
{
    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string &vertex_path, const std::string &fragment_path);
        OpenGLShader(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc);
        virtual ~OpenGLShader();

        virtual void Use() const override;
        virtual void unUse() const override;

        virtual void setInt(const std::string &name, int value) override;
        virtual void setIntArray(const std::string &name, int *values, uint32_t count) override;
        virtual void setFloat(const std::string &name, float value) override;
        virtual void setFloat2(const std::string &name, const glm::vec2 &value) override;
        virtual void setFloat3(const std::string &name, const glm::vec3 &value) override;
        virtual void setFloat4(const std::string &name, const glm::vec4 &value) override;
        virtual void setMat4(const std::string &name, const glm::mat4 &value) override;

        virtual const std::string &getName() const override { return m_Name; }

    private:
        void uploadUniformInt(const std::string &name, int value);
        void uploadUniformIntArray(const std::string &name, int *values, uint32_t count);
        void uploadUniformFloat(const std::string &name, float value);
        void uploadUniformFloat2(const std::string &name, const glm::vec2 &value);
        void uploadUniformFloat3(const std::string &name, const glm::vec3 &value);
        void uploadUniformFloat4(const std::string &name, const glm::vec4 &value);
        void uploadUniformMat3(const std::string &name, const glm::mat3 &matrix);
        void uploadUniformMat4(const std::string &name, const glm::mat4 &matrix);

        std::string readFile(const std::string &filepath);
        void compile(const std::unordered_map<GLenum, std::string> &shaderSources);

    private:
        uint32_t m_RendererID;
        std::string m_Name;
    };

} // namespace Yutrel