#include "yutrel_pch.hpp"

#include "imgui_pass.hpp"

#include "platform/Vulkan/initializers/initializers.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace Yutrel
{
    void ImguiPass::Init(RenderPassInitInfo* info)
    {
    }

    void ImguiPass::PreparePassData(Ref<struct RenderData> render_data)
    {
        m_render_data = render_data;
    }

    void ImguiPass::DrawImgui()
    {
        auto cmd_buffer = m_rhi->GetCurrentCommandBuffer();

        m_rhi->TransitionImage(cmd_buffer,
                               m_rhi->GetCurrentSwapchainImage(),
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        VkRenderingAttachmentInfo colorAttachment = vkinit::AttachmentInfo(m_rhi->GetCurrentSwapchainImageView(), nullptr, VK_IMAGE_LAYOUT_GENERAL);
        VkRenderingInfo renderInfo                = vkinit::RenderingInfo(m_rhi->GetSwapChainInfo().extent, &colorAttachment, nullptr);

        vkCmdBeginRendering(cmd_buffer, &renderInfo);

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd_buffer);

        vkCmdEndRendering(cmd_buffer);

        m_rhi->TransitionImage(cmd_buffer,
                               m_rhi->GetCurrentSwapchainImage(),
                               VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                               VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    }
} // namespace Yutrel