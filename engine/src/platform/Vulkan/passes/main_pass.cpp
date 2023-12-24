#include "yutrel_pch.hpp"

#include "main_pass.hpp"

#include "platform/Vulkan/initializers/initializers.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include <vector>

namespace Yutrel
{
    void MainPass::Init(RenderPassInitInfo* info)
    {
        InitRenderPass();
        InitFramebuffer();
        InitPipeline();
    }

    void MainPass::InitRenderPass()
    {
        // renderpass的颜色附件
        VkAttachmentDescription color_attachment{};
        color_attachment.format         = m_rhi->GetSwapChainInfo().image_format;
        color_attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // subpass的颜色附件引用
        VkAttachmentReference color_attachment_ref{};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // subpass 1个
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments    = &color_attachment_ref;

        // 创建renderpass
        VkRenderPassCreateInfo render_pass_info{};
        render_pass_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments    = &color_attachment;
        render_pass_info.subpassCount    = 1;
        render_pass_info.pSubpasses      = &subpass;

        YUTREL_ASSERT(m_rhi->CreateRenderPass(render_pass_info, &m_render_pass), "Failed to create render pass");
    }

    void MainPass::InitFramebuffer()
    {
        // 为交换链的每一个图像视图创建帧缓冲
        const auto& swapchain_info = m_rhi->GetSwapChainInfo();

        m_swapchain_framebuffers.resize(swapchain_info.image_views->size());

        VkFramebufferCreateInfo info = vkinit::FramebufferCreateInfo(m_render_pass, swapchain_info.extent);

        for (int i = 0; i < swapchain_info.image_views->size(); i++)
        {
            info.pAttachments = &(*swapchain_info.image_views)[i];

            YUTREL_ASSERT(m_rhi->CreateFramebuffer(info, &m_swapchain_framebuffers[i]), "Failed to create framebuffer");
        }
    }

    void MainPass::InitPipeline()
    {
        RHIGraphicsPipelineCreateInfo pipeline_create_info{};

        //-------------着色器模块-------------
        std::vector<unsigned char> triangle_vert_code{
#include "triangle.vert.spv.h"
        };
        std::vector<unsigned char> triangle_frag_code{
#include "triangle.frag.spv.h"
        };

        // 因为着色器中出现错误很常见，所以此处不用assert
        VkShaderModule triangle_vert_shader;
        if (!m_rhi->CreateShaderModule(triangle_vert_code, &triangle_vert_shader))
        {
            LOG_ERROR("Failed to create triangle vert shader");
        }

        VkShaderModule triangle_frag_shader;
        if (!m_rhi->CreateShaderModule(triangle_frag_code, &triangle_frag_shader))
        {
            LOG_ERROR("Failed to create triangle frag shader");
        }

        // 删除着色器模块
        m_rhi->DestroyShaderModule(triangle_vert_shader);
        m_rhi->DestroyShaderModule(triangle_frag_shader);
    }

} // namespace Yutrel