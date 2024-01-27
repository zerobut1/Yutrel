#pragma once

#include "core/macro.hpp"
#include "platform/Vulkan/asset/render_common.hpp"
#include "platform/Vulkan/vulkan_renderer.hpp"
#include "platform/Vulkan/vulkan_types.hpp"

namespace Yutrel
{
    class VulkanRHI;
    class RenderPass;

    struct RenderPipelineInitInfo
    {
        Ref<GlobalRenderData> global_render_data;
    };

    class VulkanPipeline
    {
    public:
        void Init(RenderPipelineInitInfo info);

        void Clear() {}

        void SetRHI(Ref<VulkanRHI> rhi) { m_rhi = rhi; }

        void ForwardRender();

        void PreparePassData(Ref<struct RenderData> render_data);

        VkDescriptorSetLayout GetMaterialDescriptorSetLayout();

    private:
        Ref<VulkanRHI> m_rhi;

        Ref<RenderPass> m_directional_light_pass;
        Ref<RenderPass> m_main_pass;
        Ref<RenderPass> m_imgui_pass;

        Ref<RenderPass> m_test_pass;
    };
} // namespace Yutrel