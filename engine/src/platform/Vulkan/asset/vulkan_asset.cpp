#include "yutrel_pch.hpp"

#include "vulkan_asset.hpp"

#include "platform/Vulkan/vulkan_renderer.hpp"

namespace Yutrel
{
    void VulkanAssetManager::Init(AssetManagerInitInfo info)
    {
        m_rhi                                          = info.rhi;
        m_global_render_data                           = CreateRef<GlobalRenderData>();
        m_global_render_data->scene_buffer             = m_rhi->CreateBuffer(sizeof(SceneUniformData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, true);
        m_global_render_data->directional_light_buffer = m_rhi->CreateBuffer(sizeof(DirectionalLightUniformData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, true);
    }

    Ref<VulkanMesh> VulkanAssetManager::SetVulkanMesh(Ref<Mesh> mesh)
    {
        if (!m_meshes.count(mesh))
        {
            auto vulkan_mesh = m_rhi->UploadMesh(mesh);
            m_meshes.insert({mesh, vulkan_mesh});
        }
        return m_meshes[mesh];
    }

    Ref<VulkanPBRMaterial> VulkanAssetManager::SetVulkanMaterial(Ref<Material> material)
    {
        if (!m_materials.count(material))
        {
            auto vulkan_material = CreateRef<VulkanPBRMaterial>();

            // 加载数据到uniform buffer
            vulkan_material->uniform_buffer = m_rhi->UploadMaterialData(material);

            // 加载纹理到GPU
            if (material->base_color_texture && !m_textures.count(material->base_color_texture))
            {
                m_textures.insert({material->base_color_texture, m_rhi->UploadTexture(material->base_color_texture)});
            }
            if (material->metallic_roughness_texture && !m_textures.count(material->metallic_roughness_texture))
            {
                m_textures.insert({material->metallic_roughness_texture, m_rhi->UploadTexture(material->metallic_roughness_texture)});
            }
            if (material->normal_texture && !m_textures.count(material->normal_texture))
            {
                m_textures.insert({material->normal_texture, m_rhi->UploadTexture(material->normal_texture)});
            }
            vulkan_material->base_color_texture         = m_textures[material->base_color_texture];
            vulkan_material->metallic_roughness_texture = m_textures[material->metallic_roughness_texture];
            vulkan_material->normal_texture             = m_textures[material->normal_texture];

            // 分配描述符集
            m_rhi->AllocateDescriptorSets(m_material_descriptor_set_layout, &vulkan_material->descriptor_set);

            // 写描述符集
            DescriptorWriter writer;
            auto& default_data = m_rhi->GetDefaultData();
            writer.WriteBuffer(0,
                               vulkan_material->uniform_buffer.buffer,
                               sizeof(MaterialUniformData),
                               0,
                               VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            writer.WriteImage(1,
                              vulkan_material->base_color_texture.image_view,
                              default_data.default_sampler_linear,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                              VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            writer.WriteImage(2,
                              material->metallic_roughness_texture ? vulkan_material->metallic_roughness_texture.image_view : m_rhi->GetDefaultData().white_image.image_view,
                              default_data.default_sampler_linear,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                              VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            writer.WriteImage(3,
                              material->normal_texture ? vulkan_material->normal_texture.image_view : m_rhi->GetDefaultData().white_image.image_view,
                              default_data.default_sampler_linear,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                              VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

            m_rhi->UpdateDescriptorSets(writer, vulkan_material->descriptor_set);

            m_materials.insert({material, vulkan_material});
        }
        return m_materials[material];
    }

} // namespace Yutrel