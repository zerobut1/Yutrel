#include "yutrel_pch.hpp"

#include "vulkan_pipeline.hpp"

#include "function/render/renderer.hpp"
#include "platform/Vulkan/passes/imgui_pass.hpp"
#include "platform/Vulkan/passes/main_pass.hpp"
#include "platform/Vulkan/passes/test_pass.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"

#include <memory>

namespace Yutrel
{
    void VulkanPipeline::Init(RenderPipelineInitInfo info)
    {
        m_main_pass  = CreateRef<MainPass>();
        m_imgui_pass = CreateRef<ImguiPass>();
        m_test_pass  = CreateRef<TestPass>();

        m_main_pass->SetRHI(m_rhi);
        m_imgui_pass->SetRHI(m_rhi);
        m_test_pass->SetRHI(m_rhi);

        MainPassInitInfo main_init_info{};
        main_init_info.global_render_data = info.global_render_data;
        m_main_pass->Init(&main_init_info);

        ImguiPassInitInfo imgui_init_info{};
        m_imgui_pass->Init(&imgui_init_info);

        // MainPassInitInfo main_init_info{};
        // m_main_pass->Init(&main_init_info);
    }

    void VulkanPipeline::ForwardRender()
    {
        m_rhi->PrepareBeforePass();

        std::dynamic_pointer_cast<MainPass>(m_main_pass)->DrawForward();
        std::dynamic_pointer_cast<ImguiPass>(m_imgui_pass)->DrawImgui();

        m_rhi->SubmitRendering();
    }

    void VulkanPipeline::PreparePassData(Ref<RenderData> render_data)
    {
        m_main_pass->PreparePassData(render_data);
        m_imgui_pass->PreparePassData(render_data);
        m_test_pass->PreparePassData(render_data);
    }

    VkDescriptorSetLayout VulkanPipeline::GetMaterialDescriptorSetLayout()
    {
        return std::reinterpret_pointer_cast<MainPass>(m_main_pass)->GetMaterialDescriptorSetLayout();
    }

} // namespace Yutrel