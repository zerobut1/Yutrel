#pragma once

#include "platform/Vulkan/asset/vulkan_material.hpp"
#include "platform/Vulkan/asset/vulkan_mesh.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"

#include <stdint.h>
#include <unordered_map>

namespace Yutrel
{
    class VulkanAssetManager
    {
    public:
        void SetRHI(Ref<VulkanRHI> rhi) { m_rhi = rhi; }

        Ref<VulkanMesh> SetVulkanMesh(Ref<Mesh> mesh);

        Ref<VulkanPBRMaterial> SetVulkanMaterial(Ref<Material> material);

    private:
        Ref<VulkanRHI> m_rhi;

        std::unordered_map<Ref<Mesh>, Ref<VulkanMesh>> m_meshes;
        std::unordered_map<Ref<Material>, Ref<VulkanPBRMaterial>> m_materials;
    };
} // namespace Yutrel