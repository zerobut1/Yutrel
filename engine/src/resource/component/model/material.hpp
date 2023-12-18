#pragma once

#include "resource/asset/shader.hpp"
#include "resource/asset/texture.hpp"

#include <glm/glm.hpp>

#include <stdint.h>
#include <string>
#include <unordered_map>
#include <variant>

namespace Yutrel
{
    using uniform_variant = std::variant<int, int*, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;

    class Material
    {
    public:
        static Ref<Material> Create();

    public:
        Material()          = default;
        virtual ~Material() = default;

        void Bind(Ref<Shader> shader);
        // void Unbind(Shader* shader);

        void SetUniform(const std::string& name, const uniform_variant value);
        void SetTexture(uint32_t unit, Ref<Texture> texture);

        bool IsBlend();

    private:
        std::unordered_map<std::string, uniform_variant> m_uniforms;
        std::unordered_map<uint32_t, Ref<Texture>> m_textures;
    };

} // namespace Yutrel