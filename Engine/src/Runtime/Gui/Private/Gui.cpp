#include "Gui.h"

#include "Renderer.h"
#include "Swapchain.h"
#include "Window.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace Yutrel
{
    Gui::Gui(const CreateInfo& info)
        : m_renderer(info.renderer),
          m_window(info.window),
          m_swapchain(info.swapchain)
    {
        init(info);
    }

    Gui::~Gui()
    {
        shutdown();
    }

    void Gui::init(const CreateInfo& info)
    {
        // descriptor pool
        std::vector<vk::DescriptorPoolSize> sizes{
            {vk::DescriptorType::eCombinedImageSampler, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE},
        };

        auto pool_ci =
            vk::DescriptorPoolCreateInfo()
                .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
                .setMaxSets(IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE)
                .setPoolSizes(sizes);

        m_descriptor_pool = m_renderer->getContext()->getDevice().createDescriptorPool(pool_ci);

        // Imgui init
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(2.0f);
        style.FontScaleDpi = 2.0f;

        io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");

        ImGui_ImplGlfw_InitForVulkan(m_window->getWindow(), true);
        vk::Format swapchain_format = m_swapchain->getFormat();
        ImGui_ImplVulkan_InitInfo init_info{
            .Instance                    = static_cast<VkInstance>(m_renderer->getContext()->getInstance()),
            .PhysicalDevice              = static_cast<VkPhysicalDevice>(m_renderer->getContext()->getGPU()),
            .Device                      = static_cast<VkDevice>(m_renderer->getContext()->getDevice()),
            .QueueFamily                 = m_renderer->getContext()->getMainQueueIndex(),
            .Queue                       = static_cast<VkQueue>(m_renderer->getContext()->getMainQueue()),
            .DescriptorPool              = static_cast<VkDescriptorPool>(m_descriptor_pool),
            .MinImageCount               = 2,
            .ImageCount                  = 2,
            .MSAASamples                 = VK_SAMPLE_COUNT_1_BIT,
            .UseDynamicRendering         = true,
            .PipelineRenderingCreateInfo = vk::PipelineRenderingCreateInfo()
                                               .setColorAttachmentCount(1)
                                               .setColorAttachmentFormats(swapchain_format),
        };
        ImGui_ImplVulkan_Init(&init_info);
    }

    void Gui::shutdown()
    {
        ImGui_ImplVulkan_Shutdown();
    }

    void Gui::updateUI(const std::function<void()>& onUIUpdate)
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (onUIUpdate)
        {
            onUIUpdate();
        }

        ImGui::Render();
    }

    void Gui::drawUI(vk::CommandBuffer cmd_buffer)
    {
        m_renderer->transitionImageLayout(cmd_buffer,
                                          m_swapchain->getCurrentImage(),
                                          vk::ImageLayout::eUndefined,
                                          vk::ImageLayout::eColorAttachmentOptimal);

        auto color_attachment =
            vk::RenderingAttachmentInfo()
                .setImageView(m_swapchain->getCurrentImageView())
                .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
                .setLoadOp(vk::AttachmentLoadOp::eLoad)
                .setStoreOp(vk::AttachmentStoreOp::eStore);

        auto render_info =
            vk::RenderingInfo()
                .setRenderArea(vk::Rect2D({0, 0}, m_swapchain->getExtent()))
                .setLayerCount(1)
                .setColorAttachments(color_attachment);

        cmd_buffer.beginRendering(render_info);

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd_buffer);

        cmd_buffer.endRendering();

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        m_renderer->transitionImageLayout(cmd_buffer,
                                          m_swapchain->getCurrentImage(),
                                          vk::ImageLayout::eColorAttachmentOptimal,
                                          vk::ImageLayout::ePresentSrcKHR);
    }

} // namespace Yutrel