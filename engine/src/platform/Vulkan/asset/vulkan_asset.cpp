#include "yutrel_pch.hpp"

#include "vulkan_asset.hpp"

namespace Yutrel
{
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
            vulkan_material->base_color_texture = m_rhi->UploadTexture(material->base_color_texture);

            m_materials.insert({material, vulkan_material});
        }
        return m_materials[material];
    }

} // namespace Yutrel