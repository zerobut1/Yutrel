#include "yutrel_pch.hpp"

#include "imgui_pass.hpp"

#include "platform/Vulkan/rhi/vulkan_rhi.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace Yutrel
{
    void ImguiPass::Init(RenderPassInitInfo* info)
    {
        RenderPass::Init(info);
    }

    void ImguiPass::DrawImgui()
    {
        auto cmd_buffer = m_rhi->GetCurrentCommandBuffer();

        m_rhi->TransitionImage(cmd_buffer,
                               m_rhi->GetCurrentSwapchainImage(),
                               vk::ImageLayout::eTransferDstOptimal,
                               vk::ImageLayout::eColorAttachmentOptimal);

        auto color_attachment =
            vk::RenderingAttachmentInfo()
                .setImageView(m_rhi->GetCurrentSwapchainImageView())
                .setImageLayout(vk::ImageLayout::eGeneral)
                .setLoadOp(vk::AttachmentLoadOp::eLoad)
                .setStoreOp(vk::AttachmentStoreOp::eStore);

        auto render_info =
            vk::RenderingInfo()
                .setRenderArea(vk::Rect2D({0, 0}, m_rhi->GetSwapChainExtent()))
                .setLayerCount(1)
                .setColorAttachments(color_attachment);

        cmd_buffer.beginRendering(render_info);

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd_buffer);

        cmd_buffer.endRendering();

        m_rhi->TransitionImage(cmd_buffer,
                               m_rhi->GetCurrentSwapchainImage(),
                               vk::ImageLayout::eColorAttachmentOptimal,
                               vk::ImageLayout::ePresentSrcKHR);
    }
} // namespace Yutrel