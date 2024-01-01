#include "yutrel_pch.hpp"

#include "vulkan_pipeline.hpp"

#include "function/render/renderer.hpp"
#include "platform/Vulkan/passes/main_pass.hpp"
#include "platform/Vulkan/passes/test_pass.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"

#include <memory>

namespace Yutrel
{
    void VulkanPipeline::Init(RenderPipelineInitInfo info)
    {
        m_main_pass = CreateRef<MainPass>();
        m_test_pass = CreateRef<TestPass>();

        m_main_pass->SetRHI(m_rhi);
        m_test_pass->SetRHI(m_rhi);

        // MainPassInitInfo main_init_info{};
        // m_main_pass->Init(&main_init_info);

        TestPassInitInfo test_init_info{};
        m_test_pass->Init(&test_init_info);
    }

    void VulkanPipeline::ForwardRender()
    {
        // todo recreate swapchian
        m_rhi->PrepareBeforePass();

        // std::dynamic_pointer_cast<MainPass>(m_main_pass)->DrawForward();
        std::dynamic_pointer_cast<TestPass>(m_test_pass)->DrawForward();

        m_rhi->SubmitRendering();
    }

    void VulkanPipeline::PreparePassData(Ref<RenderData> render_data)
    {
        m_main_pass->PreparePassData(render_data);
        m_test_pass->PreparePassData(render_data);
    }

} // namespace Yutrel