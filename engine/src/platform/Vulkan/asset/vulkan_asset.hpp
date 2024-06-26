#pragma once

#include "platform/Vulkan/asset/vulkan_material.hpp"
#include "platform/Vulkan/asset/vulkan_mesh.hpp"
#include "platform/Vulkan/asset/vulkan_skybox.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"
#include "platform/Vulkan/vulkan_types.hpp"

#include <unordered_map>
#include <vulkan/vulkan_handles.hpp>

namespace Yutrel
{
    struct AssetManagerInitInfo
    {
        Ref<VulkanRHI> rhi;
    };

    class VulkanAssetManager
    {
    public:
        void Init(AssetManagerInitInfo info);

        Ref<VulkanMesh> SetVulkanMesh(Ref<Mesh> mesh);

        Ref<VulkanPBRMaterial> SetVulkanMaterial(Ref<Material> material);

        Ref<VulkanSkybox> SetSkybox(Skybox skybox);

        void SetMaterialDescriptorSetLayout(vk::DescriptorSetLayout layout) { m_material_descriptor_set_layout = layout; }

        AllocatedImage UploadTexture(Ref<Texture> texture);

    public:
        struct
        {
            AllocatedImage white_texture;
            AllocatedImage error_texture;

            vk::Sampler linear_sampler;
            vk::Sampler nearset_sampler;
        } m_default_data;

    private:
        void InitDefaultData();

        Ref<VulkanMesh> UploadMesh(Ref<Mesh> mesh);

        AllocatedBuffer UploadMaterialData(Ref<Material> material);

    private:
        Ref<VulkanRHI> m_rhi;

        vk::DescriptorSetLayout m_material_descriptor_set_layout;

        std::unordered_map<Ref<Mesh>, Ref<VulkanMesh>> m_meshes;
        std::unordered_map<Ref<Texture>, AllocatedImage> m_textures;
        std::unordered_map<Ref<Material>, Ref<VulkanPBRMaterial>> m_materials;
    };
} // namespace Yutrel