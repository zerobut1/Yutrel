#pragma once

#include "platform/Vulkan/asset/vulkan_material.hpp"
#include "platform/Vulkan/asset/vulkan_mesh.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"
#include "platform/Vulkan/vulkan_types.hpp"

#include <stdint.h>
#include <unordered_map>

namespace Yutrel
{
    struct AssetManagerInitInfo
    {};

    class VulkanAssetManager
    {
    public:
        void Init(AssetManagerInitInfo info);

        void SetRHI(Ref<VulkanRHI> rhi) { m_rhi = rhi; }

        Ref<VulkanMesh> SetVulkanMesh(Ref<Mesh> mesh);

        Ref<VulkanPBRMaterial> SetVulkanMaterial(Ref<Material> material);

        void SetMaterialDescriptorSetLayout(VkDescriptorSetLayout layout) { m_material_descriptor_set_layout = layout; }

        Ref<GlobalRenderData> GetGlobalRenderData() const { return m_global_render_data; }

    private:
        Ref<VulkanRHI> m_rhi;

        VkDescriptorSetLayout m_material_descriptor_set_layout;

        std::unordered_map<Ref<Mesh>, Ref<VulkanMesh>> m_meshes;
        std::unordered_map<Ref<Material>, Ref<VulkanPBRMaterial>> m_materials;

        Ref<GlobalRenderData> m_global_render_data;
    };
} // namespace Yutrel