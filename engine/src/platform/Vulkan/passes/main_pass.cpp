#include "yutrel_pch.hpp"

#include "main_pass.hpp"

#include "platform/Vulkan/initializers/initializers.hpp"
#include "platform/Vulkan/mesh/mesh.hpp"
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

    void MainPass::DrawForward()
    {
        // 设定清除颜色
        VkClearValue clear_value;
        float flash       = abs(sin(m_rhi->GetCurrentFrameCount() / 144.0f));
        clear_value.color = {{0.0f, 0.0f, flash, 1.0f}};

        //----------Renderpass--------------
        VkRenderPassBeginInfo render_pass_info = vkinit::RenderPassBeginInfo(m_render_pass, m_rhi->GetSwapChainInfo().extent, m_swapchain_framebuffers[m_rhi->GetCurrentSwapchainImageIndex()]);
        render_pass_info.clearValueCount       = 1;
        render_pass_info.pClearValues          = &clear_value;

        m_rhi->CmdBeginRenderPass(m_rhi->GetCurrentCommandBuffer(), &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        m_rhi->CmdBindPipeline(m_rhi->GetCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[0].pipeline);

        VkDeviceSize offset = 0;
        m_rhi->CmdBindVertexBuffers(m_rhi->GetCurrentCommandBuffer(), 0, 1, &m_triangle_mesh->vertex_buffer.buffer, &offset);

        m_rhi->CmdDraw(m_rhi->GetCurrentCommandBuffer(), m_triangle_mesh->vertices.size(), 1, 0, 0);

        m_rhi->CmdEndRenderPass(m_rhi->GetCurrentCommandBuffer());
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

        YUTREL_ASSERT(m_rhi->CreateRenderPass(&render_pass_info, &m_render_pass), "Failed to create render pass");
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

            YUTREL_ASSERT(m_rhi->CreateFramebuffer(&info, &m_swapchain_framebuffers[i]), "Failed to create framebuffer");
        }
    }

    void MainPass::InitPipeline()
    {
        m_render_pipelines.resize(1);

        //-------------着色器模块-------------
        // clang-format off
        std::vector<unsigned char> triangle_vert_code{
            #include "triangle.vert.spv.h"
        };
        std::vector<unsigned char> triangle_frag_code{
            #include "triangle.frag.spv.h"
        };
        // clang-format on

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

        //-----------管线布局-------------
        VkPipelineLayoutCreateInfo layout_info = vkinit::PipelineLayoutCreateInfo();

        YUTREL_ASSERT(m_rhi->CreatePipelineLayout(&layout_info, &m_render_pipelines[0].layout), "Failed to create pipeline layout");

        //----------顶点状态-------------
        VertexInputDescription vertex_description = Vertex::GetVertexDescription();

        //-----------视口状态------------
        VkPipelineViewportStateCreateInfo viewport_state_info{};
        viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state_info.pNext = nullptr;

        // 暂时只有一个视口一个剪刀
        auto swapchain_info               = m_rhi->GetSwapChainInfo();
        viewport_state_info.viewportCount = 1;
        viewport_state_info.pViewports    = &swapchain_info.viewport;
        viewport_state_info.scissorCount  = 1;
        viewport_state_info.pScissors     = &swapchain_info.scissor;

        //------------颜色混合-------------
        VkPipelineColorBlendAttachmentState color_blend_attachment = vkinit::ColorBlendAttachmentState();
        // 暂设为不混合
        VkPipelineColorBlendStateCreateInfo color_blending_info{};
        color_blending_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending_info.pNext = nullptr;

        color_blending_info.logicOpEnable   = VK_FALSE;
        color_blending_info.logicOp         = VK_LOGIC_OP_COPY;
        color_blending_info.attachmentCount = 1;
        color_blending_info.pAttachments    = &color_blend_attachment;

        //------------创建管线-----------
        RHIGraphicsPipelineCreateInfo pipeline_create_info{};
        // 布局
        pipeline_create_info.pipeline_layout = m_render_pipelines[0].layout;
        // 着色器
        pipeline_create_info.shader_stages.push_back(vkinit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, triangle_vert_shader));
        pipeline_create_info.shader_stages.push_back(vkinit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, triangle_frag_shader));
        // 顶点输入
        pipeline_create_info.vertex_input                                 = vkinit::VertexInputStateCreateInfo();
        pipeline_create_info.vertex_input.vertexAttributeDescriptionCount = vertex_description.attributes.size();
        pipeline_create_info.vertex_input.pVertexAttributeDescriptions    = vertex_description.attributes.data();
        pipeline_create_info.vertex_input.vertexBindingDescriptionCount   = vertex_description.bindings.size();
        pipeline_create_info.vertex_input.pVertexBindingDescriptions      = vertex_description.bindings.data();
        // 图元装配
        pipeline_create_info.input_assembly = vkinit::InputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        // 视口状态
        pipeline_create_info.viewport_state = viewport_state_info;
        // 光栅化
        pipeline_create_info.rasterizer = vkinit::RasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);
        // msaa
        pipeline_create_info.multisampling = vkinit::MultiSamplingStateCreateInfo();
        // 颜色混合
        pipeline_create_info.color_blend = color_blending_info;
        // render pass
        pipeline_create_info.render_pass = m_render_pass;
        pipeline_create_info.subpass     = 0;

        YUTREL_ASSERT(m_rhi->CreateGraphicsPipeline(pipeline_create_info, &m_render_pipelines[0].pipeline), "Failed to create graphics pipeline");

        //------------删除着色器模块--------------
        m_rhi->DestroyShaderModule(triangle_vert_shader);
        m_rhi->DestroyShaderModule(triangle_frag_shader);
    }

} // namespace Yutrel