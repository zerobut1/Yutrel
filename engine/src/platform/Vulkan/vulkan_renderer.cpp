#include "yutrel_pch.hpp"

#include "vulkan_renderer.hpp"

#include "platform/Vulkan/asset/vulkan_asset.hpp"
#include "platform/Vulkan/asset/vulkan_mesh.hpp"
#include "platform/Vulkan/passes/render_pass.hpp"
#include "platform/Vulkan/pipeline/vulkan_pipeline.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include "resource/component/component.hpp"
#include "resource/component/window_ui.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

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

        // 初始化资产管理
        m_asset_manager = CreateRef<VulkanAssetManager>();
        m_asset_manager->SetRHI(m_rhi);

        m_render_data = CreateRef<RenderData>();
    }

    void VulkanRenderer::Tick(Ref<SwapData> swap_data)
    {
        // 处理渲染数据
        ProcessRenderData(swap_data);

        // 重建交换链
        if (m_rhi->RequestResize())
        {
            m_rhi->ResizeSwapchain();
        }

        // Imgui
        NewImguiFrame(swap_data->ui);

        m_rhi->PrepareContext();

        m_render_pipeline->PreparePassData(m_render_data);

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

    void VulkanRenderer::ProcessRenderData(Ref<SwapData> pass_data)
    {
        // 背景颜色
        m_render_data->background = pass_data->background;

        // 物体
        for (auto pbr : pass_data->pbrs)
        {
            // mesh加载到GPU，并释放内存
            Ref<VulkanMesh> vulkan_mesh = m_asset_manager->SetVulkanMesh(pbr->mesh);

            // 材质的数据加载到GPU
            Ref<VulkanPBRMaterial> vulkan_material = m_asset_manager->SetVulkanMaterial(pbr->material);
            // Ref<VulkanPBRMaterial> vulkan_material = nullptr;

            // 存储到render_data
            auto& objects = m_render_data->objects[vulkan_material][vulkan_mesh];
            // todo transform组件
            objects.push_back(glm::mat4{0.0f});
        }
    }

    void VulkanRenderer::NewImguiFrame(Ref<WindowUI> ui)
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ui->RenderUI();

        ImGui::Render();
    }
} // namespace Yutrel