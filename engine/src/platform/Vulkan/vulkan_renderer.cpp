#include "yutrel_pch.hpp"

#include "vulkan_renderer.hpp"

#include "platform/Vulkan/passes/render_pass.hpp"
#include "platform/Vulkan/pipeline/vulkan_pipeline.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include "resource/component/component.hpp"
#include "resource/component/ui/window_ui.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <vector>

// #include "platform/Vulkan/mesh.hpp"

namespace Yutrel
{
    void VulkanRenderer::Init(RendererInitInfo info)
    {
        LOG_INFO("Initialize Vulkan Renderer");

        // 初始化RHI
        RHIInitInfo rhi_init_info;
        rhi_init_info.raw_window = info.raw_window;
        rhi_init_info.width      = info.width;
        rhi_init_info.height     = info.height;

        m_rhi = CreateRef<VulkanRHI>();
        m_rhi->Init(rhi_init_info);

        // 初始化pipeline
        RenderPipelineInitInfo pipeline_init_info{};
        m_render_pipeline = CreateRef<VulkanPipeline>();
        m_render_pipeline->SetRHI(m_rhi);
        m_render_pipeline->Init(pipeline_init_info);
    }

    void VulkanRenderer::Tick(Ref<RenderData> render_data)
    {
        m_render_data = render_data;
        ProcessRenderData();

        if (m_rhi->RequestResize())
        {
            m_rhi->ResizeSwapchain();
        }

        NewImguiFrame();

        m_rhi->PrepareContext();

        m_render_pipeline->PreparePassData(render_data);

        m_render_pipeline->ForwardRender();
    }

    void VulkanRenderer::Clear()
    {
        // 清除RHI
        if (m_rhi)
        {
            m_rhi->Clear();
        }
        m_rhi.reset();

        // 清除Pipeline
        if (m_render_pipeline)
        {
            m_render_pipeline->Clear();
        }
        m_render_pipeline.reset();
    }

    void VulkanRenderer::UpdateWindowSize(uint32_t width, uint32_t height)
    {
        m_rhi->UpdateSwapchainSize(width, height);
    }

    void VulkanRenderer::ProcessRenderData()
    {
        for (auto& pbr : m_render_data->pbrs)
        {
            if (!pbr->mesh->is_uploaded)
            {
                m_rhi->UploadMesh(pbr->mesh);

                pbr->mesh->is_uploaded = true;
            }
        }
    }

    void VulkanRenderer::NewImguiFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        m_render_data->ui->RenderUI();

        ImGui::Render();
    }
} // namespace Yutrel