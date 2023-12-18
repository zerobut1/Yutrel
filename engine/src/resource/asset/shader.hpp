/**
 * 着色器
 * 具体实现与平台有关
 * 顶点、片段、几何、计算着色器
 * 提供创建、绑定、设置值等功能
 * todo 按索引值而不是名称绑定值
 */
#pragma once

#include "core/macro.hpp"
#include "resource/asset/asset.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <stdint.h>
#include <string>

namespace Yutrel
{

    typedef unsigned int GLenum;
    typedef unsigned int GLbitfield;

    // Shader
    class Shader : public Asset
    {
    public:
        static Ref<Shader> Create(const std::string& vertex_path, const std::string& fragment_path);
        static Ref<Shader> Create(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path);

    public:
        virtual ~Shader() = default;

        virtual void Bind() const   = 0;
        virtual void Unbind() const = 0;

        // 后面会改成自己的数学库
        virtual void SetInt(const std::string& name, int value)                                  = 0;
        virtual void SetIntArray(const std::string& name, const int* values, uint32_t count)     = 0;
        virtual void SetFloat(const std::string& name, float value)                              = 0;
        virtual void SetFloatArray(const std::string& name, const float* values, uint32_t count) = 0;
        virtual void SetFloat2(const std::string& name, const glm::vec2& value)                  = 0;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value)                  = 0;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value)                  = 0;
        virtual void SetMat4(const std::string& name, const glm::mat4& value)                    = 0;

        virtual const std::string& GetName() const = 0;
    };

    // Compute Shader
    class ComputeShader : public Asset
    {
    public:
        static Ref<ComputeShader> Create(const std::string& shader_path);

    public:
        virtual ~ComputeShader() = default;

        virtual void Bind() const   = 0;
        virtual void Unbind() const = 0;

        virtual void Dispatch(uint32_t nx, uint32_t ny, uint32_t nz = 1) const = 0;
        virtual void SyncWait(GLbitfield barriers) const                       = 0;

        virtual void SetInt(const std::string& name, int value)                        = 0;
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
        virtual void SetFloat(const std::string& name, float value)                    = 0;
        virtual void SetFloat2(const std::string& name, const glm::vec2& value)        = 0;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value)        = 0;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value)        = 0;
        virtual void SetMat4(const std::string& name, const glm::mat4& value)          = 0;

        virtual const std::string& GetName() const = 0;
    };
} // namespace Yutrel