#include "yutrel_pch.hpp"

#include "material.hpp"

namespace Yutrel
{
    Ref<Material> Material::Create()
    {
        return CreateRef<Material>();
    }

    void Material::Bind(Ref<Shader> shader)
    {
        shader->Bind();
        // 设定uniform
        for (auto& [name, value] : m_uniforms)
        {
            if (std::holds_alternative<int>(value))
            {
                int int_value = std::get<int>(value);
                // 处理int类型的值
                shader->SetInt(name, int_value);
            }
            else if (std::holds_alternative<float>(value))
            {
                float float_value = std::get<float>(value);
                // 处理float类型的值
                shader->SetFloat(name, float_value);
            }
            else if (std::holds_alternative<glm::vec2>(value))
            {
                glm::vec2 vec2_value = std::get<glm::vec2>(value);
                // 处理vec2类型的值
                shader->SetFloat2(name, vec2_value);
            }
            else if (std::holds_alternative<glm::vec3>(value))
            {
                glm::vec3 vec3_value = std::get<glm::vec3>(value);
                // 处理vec3类型的值
                shader->SetFloat3(name, vec3_value);
            }
            else if (std::holds_alternative<glm::vec4>(value))
            {
                glm::vec4 vec4_value = std::get<glm::vec4>(value);
                // 处理vec4类型的值
                shader->SetFloat4(name, vec4_value);
            }
            else if (std::holds_alternative<glm::mat4>(value))
            {
                glm::mat4 mat4_value = std::get<glm::mat4>(value);
                // 处理mat4类型的值
                shader->SetMat4(name, mat4_value);
            }
        }

        // 设定texture
        for (auto& [unit, texture] : m_textures)
        {
            texture->Bind(unit);
        }
    }

    void Material::SetTexture(uint32_t unit, Ref<Texture> texture)
    {
        m_textures.insert_or_assign(unit, texture);
        // m_textures[unit] = texture;
    }

    void Material::SetUniform(const std::string& name, const uniform_variant value)
    {
        // m_uniforms.insert_or_assign(name, value);
        m_uniforms[name] = value;
    }

    bool Material::IsBlend()
    {
        if (std::holds_alternative<float>(m_uniforms["u_blend_toggle"]))
        {
            if (std::get<float>(m_uniforms["u_blend_toggle"]) == 1.0f)
            {
                return true;
            }
        }
        return false;
    }

} // namespace Yutrel