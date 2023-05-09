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

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void SetInt(const std::string &name, int value) override;
        virtual void SetIntArray(const std::string &name, int *values, uint32_t count) override;
        virtual void SetFloat(const std::string &name, float value) override;
        virtual void SetFloat2(const std::string &name, const glm::vec2 &value) override;
        virtual void SetFloat3(const std::string &name, const glm::vec3 &value) override;
        virtual void SetFloat4(const std::string &name, const glm::vec4 &value) override;
        virtual void SetMat4(const std::string &name, const glm::mat4 &value) override;

        virtual const std::string &GetName() const override { return m_Name; }

        void UploadUniformInt(const std::string &name, int value);
        void UploadUniformIntArray(const std::string &name, int *values, uint32_t count);

        void UploadUniformFloat(const std::string &name, float value);
        void UploadUniformFloat2(const std::string &name, const glm::vec2 &value);
        void UploadUniformFloat3(const std::string &name, const glm::vec3 &value);
        void UploadUniformFloat4(const std::string &name, const glm::vec4 &value);

        void UploadUniformMat3(const std::string &name, const glm::mat3 &matrix);
        void UploadUniformMat4(const std::string &name, const glm::mat4 &matrix);

    private:
        std::string Readfile(const std::string &filepath);
        void Compile(const std::unordered_map<GLenum, std::string> &shaderSources);

        uint32_t m_RendererID;
        std::string m_Name;
    };

} // namespace Yutrel