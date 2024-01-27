#include "yutrel_pch.hpp"

#include "vulkan_renderer.hpp"

#include "core/application/application.hpp"
#include "platform/Vulkan/asset/vulkan_asset.hpp"
#include "platform/Vulkan/asset/vulkan_mesh.hpp"
#include "platform/Vulkan/passes/render_pass.hpp"
#include "platform/Vulkan/pipeline/vulkan_pipeline.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include "resource/component/component.hpp"
#include "resource/component/window_ui.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <vulkan/vulkan_core.h>

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

        m_asset_manager = CreateRef<VulkanAssetManager>();
        m_asset_manager->SetRHI(m_rhi);
        m_asset_manager->Init(asset_init_info);

        // 初始化pipeline
        RenderPipelineInitInfo pipeline_init_info{};
        pipeline_init_info.global_render_data = m_asset_manager->GetGlobalRenderData();

        m_render_pipeline = CreateRef<VulkanPipeline>();
        m_render_pipeline->SetRHI(m_rhi);
        m_render_pipeline->Init(pipeline_init_info);

        // 初始化渲染数据
        m_render_data = CreateRef<RenderData>();

        // 设定材质的描述符集布局
        m_asset_manager->SetMaterialDescriptorSetLayout(m_render_pipeline->GetMaterialDescriptorSetLayout());
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

        // 渲染context
        m_rhi->PrepareContext();

        // 将渲染数据传递给管线
        m_render_pipeline->PreparePassData(m_render_data);

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

    void VulkanRenderer::ProcessRenderData(Ref<SwapData> pass_data)
    {
        // 场景信息加载到uniform buffer
        VkExtent2D swapchain_extent = m_rhi->GetSwapChainInfo().extent;
        SceneUniformData scene_data{};
        scene_data      = SceneUniformData{};
        scene_data.view = pass_data->view_matrix;
        scene_data.proj = glm::perspective(glm::radians(70.f), (float)swapchain_extent.width / (float)swapchain_extent.height, 10000.0f, 0.1f);
        scene_data.proj[1][1] *= -1;
        scene_data.view_proj          = scene_data.proj * scene_data.view;
        scene_data.view_position      = pass_data->view_position;
        scene_data.sunlight_direction = glm::vec4(pass_data->directional_light.direction, pass_data->directional_light.intensity);
        scene_data.sunlight_color     = glm::vec4(pass_data->directional_light.color, 1.0f);

        *reinterpret_cast<SceneUniformData*>(m_asset_manager->GetGlobalRenderData()->scene_buffer.info.pMappedData) = scene_data;

        // 平行光MVP矩阵
        glm::mat4 directional_light_view     = glm::lookAt(pass_data->directional_light.direction, glm::vec3(0.0f), glm::vec3(0, 1, 0));
        glm::mat4 directional_light_proj     = glm::perspective(glm::radians(45.0f), 1.0f, 10000.0f, 0.1f);
        m_render_data->directional_light_MVP = directional_light_proj * directional_light_view * glm::mat4(1.0f);

        // 物体
        for (auto object : pass_data->objects)
        {
            // mesh加载到GPU，并释放内存
            Ref<VulkanMesh> vulkan_mesh = m_asset_manager->SetVulkanMesh(object.mesh);

            // 材质的数据加载到GPU
            Ref<VulkanPBRMaterial> vulkan_material = m_asset_manager->SetVulkanMaterial(object.material);

            // 存储到render_data
            auto& objects = m_render_data->objects[vulkan_material][vulkan_mesh];

            objects.push_back(object.transform.model_matrix);
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