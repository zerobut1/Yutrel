#include "yutrel_pch.hpp"

#include "vulkan_renderer.hpp"

#include "platform/Vulkan/asset/vulkan_asset.hpp"
#include "platform/Vulkan/asset/vulkan_mesh.hpp"
#include "platform/Vulkan/pipeline/vulkan_pipeline.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include "platform/Vulkan/scene/render_scene.hpp"
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
        RHIInitInfo rhi_init_info{};
        rhi_init_info.raw_window = info.raw_window;
        rhi_init_info.width      = info.width;
        rhi_init_info.height     = info.height;

        m_rhi = CreateRef<VulkanRHI>();
        m_rhi->Init(rhi_init_info);

        // 初始化资产管理
        AssetManagerInitInfo asset_init_info{};
        asset_init_info.rhi = m_rhi;
        m_asset_manager     = CreateRef<VulkanAssetManager>();
        m_asset_manager->Init(asset_init_info);

        // 初始化渲染场景
        RenderSceneInitInfo scene_init_info{};
        scene_init_info.asset_manager = m_asset_manager;
        m_render_scene                = CreateRef<RenderScene>();
        m_render_scene->Init(scene_init_info);

        // 初始化pipeline
        RenderPipelineInitInfo pipeline_init_info{};
        pipeline_init_info.rhi           = m_rhi;
        pipeline_init_info.asset_manager = m_asset_manager;
        pipeline_init_info.render_scene  = m_render_scene;
        m_render_pipeline                = CreateRef<VulkanPipeline>();
        m_render_pipeline->Init(pipeline_init_info);

        // 设定材质的描述符集布局
        m_asset_manager->SetMaterialDescriptorSetLayout(m_render_pipeline->GetMaterialDescriptorSetLayout());
    }

    void VulkanRenderer::Tick(Ref<SwapData> swap_data)
    {
        swap_data->aspect_radio = (float)m_rhi->GetSwapChainExtent().width / (float)m_rhi->GetSwapChainExtent().height;
        // 处理渲染数据
        m_render_scene->ProcessRenderData(swap_data);

        // 重建交换链
        if (m_rhi->RequestResize())
        {
            m_rhi->ResizeSwapchain();
        }

        // Imgui
        NewImguiFrame(swap_data->ui);

        // 前向渲染
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

    void VulkanRenderer::NewImguiFrame(Ref<WindowUI> ui)
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ui->RenderUI();

        ImGui::Render();
    }
} // namespace Yutrel