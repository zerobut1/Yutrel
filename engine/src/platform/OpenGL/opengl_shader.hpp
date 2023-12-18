#pragma once

#include "resource/asset/shader.hpp"

#include <glad/glad.h>

#include <stdint.h>
#include <string>
#include <unordered_map>

typedef unsigned int GLenum;

namespace Yutrel
{
    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader() = default;
        OpenGLShader(const std::string& vertex_path, const std::string& fragment_path);
        OpenGLShader(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path);
        virtual ~OpenGLShader();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void SetInt(const std::string& name, int value) override;
        virtual void SetIntArray(const std::string& name, const int* values, uint32_t count) override;
        virtual void SetFloat(const std::string& name, float value) override;
        virtual void SetFloatArray(const std::string& name, const float* values, uint32_t count) override;
        virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
        virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

        virtual const std::string& GetName() const override { return m_name; }

    private:
        void UploadUniformInt(const std::string& name, int value);
        void UploadUniformIntArray(const std::string& name, const int* values, uint32_t count);
        void UploadUniformFloat(const std::string& name, float value);
        void UploadUniformFloatArray(const std::string& name, const float* values, uint32_t count);
        void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
        void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
        void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
        void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
        void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

        std::string ReadFile(const std::string& filepath);
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

    private:
        std::string m_name;
    };

    class OpenGLComputeShader : public ComputeShader
    {
    public:
        OpenGLComputeShader(const std::string& shader_path);
        virtual ~OpenGLComputeShader();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void Dispatch(uint32_t nx, uint32_t ny, uint32_t nz = 1) const override;
        virtual void SyncWait(GLbitfield barriers = GL_SHADER_STORAGE_BARRIER_BIT) const override;

        virtual void SetInt(const std::string& name, int value) override;
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
        virtual void SetFloat(const std::string& name, float value) override;
        virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
        virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

        virtual const std::string& GetName() const override { return m_name; }

    private:
        void UploadUniformInt(const std::string& name, int value);
        void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);
        void UploadUniformFloat(const std::string& name, float value);
        void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
        void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
        void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
        void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
        void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

        std::string ReadFile(const std::string& filepath);
        void Compile(const std::string& shader_src);

    private:
        std::string m_name;
    };

} // namespace Yutrel