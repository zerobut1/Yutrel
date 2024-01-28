#include "yutrel_pch.hpp"

#include "vulkan_pipeline.hpp"

#include "function/render/renderer.hpp"
#include "platform/Vulkan/passes/debug_draw_pass.hpp"
#include "platform/Vulkan/passes/directional_light_pass.hpp"
#include "platform/Vulkan/passes/imgui_pass.hpp"
#include "platform/Vulkan/passes/main_pass.hpp"
#include "platform/Vulkan/passes/test_pass.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"

#include <memory>

namespace Yutrel
{
    void VulkanPipeline::Init(RenderPipelineInitInfo info)
    {
        m_directional_light_pass = CreateRef<DirectionalLightPass>();
        m_main_pass              = CreateRef<MainPass>();
        m_imgui_pass             = CreateRef<ImguiPass>();
        m_test_pass              = CreateRef<TestPass>();
        m_debug_draw_pass        = CreateRef<DebugDrawPass>();

        m_directional_light_pass->SetRHI(m_rhi);
        m_main_pass->SetRHI(m_rhi);
        m_imgui_pass->SetRHI(m_rhi);
        m_test_pass->SetRHI(m_rhi);
        m_debug_draw_pass->SetRHI(m_rhi);

        DirectionalLightPassInitInfo directional_light_init_info{};
        directional_light_init_info.global_render_data = info.global_render_data;
        m_directional_light_pass->Init(&directional_light_init_info);

        MainPassInitInfo main_init_info{};
        main_init_info.global_render_data          = info.global_render_data;
        main_init_info.directional_light_shadowmap = std::static_pointer_cast<DirectionalLightPass>(m_directional_light_pass)->depth_image;
        main_init_info.shadowmap_sampler           = std::static_pointer_cast<DirectionalLightPass>(m_directional_light_pass)->depth_sampler;
        m_main_pass->Init(&main_init_info);

        ImguiPassInitInfo imgui_init_info{};
        m_imgui_pass->Init(&imgui_init_info);

        DebugDrawPassInitInfo debug_init_info{};
        debug_init_info.image_to_draw = std::static_pointer_cast<DirectionalLightPass>(m_directional_light_pass)->depth_image;
        m_debug_draw_pass->Init(&debug_init_info);
    }

    void VulkanPipeline::ForwardRender()
    {
        m_rhi->PrepareBeforePass();

        std::dynamic_pointer_cast<DirectionalLightPass>(m_directional_light_pass)->DrawForward();
        std::dynamic_pointer_cast<MainPass>(m_main_pass)->DrawForward();
        std::dynamic_pointer_cast<ImguiPass>(m_imgui_pass)->DrawImgui();
        // std::dynamic_pointer_cast<DebugDrawPass>(m_debug_draw_pass)->DrawForward();

        m_rhi->SubmitRendering();
    }

    void VulkanPipeline::PreparePassData(Ref<RenderData> render_data)
    {
        m_directional_light_pass->PreparePassData(render_data);
        m_main_pass->PreparePassData(render_data);
        m_imgui_pass->PreparePassData(render_data);
        m_test_pass->PreparePassData(render_data);
        m_debug_draw_pass->PreparePassData(render_data);
    }

    VkDescriptorSetLayout VulkanPipeline::GetMaterialDescriptorSetLayout()
    {
        return std::reinterpret_pointer_cast<MainPass>(m_main_pass)->GetMaterialDescriptorSetLayout();
    }

} // namespace Yutrel