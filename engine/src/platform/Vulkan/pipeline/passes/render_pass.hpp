#pragma once

#include "core/macro.hpp"
#include "platform/Vulkan/asset/render_common.hpp"
#include "platform/Vulkan/scene/render_scene.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"
#include "platform/Vulkan/vulkan_types.hpp"

#include <vector>

namespace Yutrel
{
    class VulkanRHI;
    struct Mesh;

    struct RenderPassInitInfo
    {
        Ref<VulkanRHI> rhi;
        Ref<VulkanAssetManager> asset_manager;
        Ref<RenderScene> render_scene;
    };

    class RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info);

    protected:
        Ref<VulkanRHI> m_rhi;
        Ref<VulkanAssetManager> m_asset_manager;
        Ref<RenderScene> m_render_scene;

        struct PipelineBase
        {
            VkPipelineLayout layout;
            VkPipeline pipeline;
        };
        std::vector<PipelineBase> m_pipelines;

        struct Descriptor
        {
            VkDescriptorSetLayout layout;
            VkDescriptorSet set;
        };
        std::vector<Descriptor> m_descriptor_infos;
    };
} // namespace Yutrel