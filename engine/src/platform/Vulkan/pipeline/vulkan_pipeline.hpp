#pragma once

#include "core/macro.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"
#include <vulkan/vulkan_handles.hpp>

namespace Yutrel
{
    class VulkanRHI;
    class RenderPass;
    class RenderScene;

    struct RenderPipelineInitInfo
    {
        Ref<VulkanRHI> rhi;
        Ref<VulkanAssetManager> asset_manager;
        Ref<RenderScene> render_scene;
    };

    class VulkanPipeline
    {
    public:
        void Init(RenderPipelineInitInfo info);

        void Clear() {}

        void ForwardRender();

        vk::DescriptorSetLayout GetMaterialDescriptorSetLayout();

    private:
        Ref<VulkanRHI> m_rhi;
        Ref<VulkanAssetManager> m_asset_manager;
        Ref<RenderScene> m_render_scene;

        Ref<RenderPass> m_directional_light_pass;
        Ref<RenderPass> m_main_pass;
        Ref<RenderPass> m_imgui_pass;

        Ref<RenderPass> m_base_pass;
        Ref<RenderPass> m_lighting_pass;
    };
} // namespace Yutrel