#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <string>

namespace Yutrel
{
    // shader类
    class Shader
    {
    public:
        static std::shared_ptr<Shader> Create(const std::string &vertex_path, const std::string &fragment_path);
        static std::shared_ptr<Shader> Create(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc);

        virtual ~Shader() = default;

        virtual void Use() const   = 0;
        virtual void unUse() const = 0;

        // 后面会改成自己的数学库
        virtual void setInt(const std::string &name, int value)                        = 0;
        virtual void setIntArray(const std::string &name, int *values, uint32_t count) = 0;
        virtual void setFloat(const std::string &name, float value)                    = 0;
        virtual void setFloat2(const std::string &name, const glm::vec2 &value)        = 0;
        virtual void setFloat3(const std::string &name, const glm::vec3 &value)        = 0;
        virtual void setFloat4(const std::string &name, const glm::vec4 &value)        = 0;
        virtual void setMat4(const std::string &name, const glm::mat4 &value)          = 0;

        virtual const std::string &getName() const = 0;
    };
} // namespace Yutrel