#include "yutrel_pch.hpp"

#include "vulkan_utils.hpp"

namespace Yutrel
{
    void RHIDynamicPipelineCreateInfo::Clear()
    {
        input_assembly       = {};
        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

        rasterizer       = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

        color_blend_attachment = {};

        multisampling       = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

        pipeline_layout = {};

        depth_stencil       = {};
        depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

        render_info       = {};
        render_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;

        shader_stages.clear();
    }

    void RHIDynamicPipelineCreateInfo::SetShaders(VkShaderModule vertex_shader, VkShaderModule fragment_shader)
    {
        shader_stages.clear();

        // 顶点着色器
        VkPipelineShaderStageCreateInfo info{};
        info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info.pNext  = nullptr;
        info.stage  = VK_SHADER_STAGE_VERTEX_BIT;
        info.module = vertex_shader;
        info.pName  = "main";
        shader_stages.push_back(info);

        // 片段着色器
        info.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
        info.module = fragment_shader;
        shader_stages.push_back(info);
    }

    void RHIDynamicPipelineCreateInfo::SetInputTopolygy(VkPrimitiveTopology topology)
    {
        input_assembly.topology               = topology;
        input_assembly.primitiveRestartEnable = VK_FALSE;
    }

    void RHIDynamicPipelineCreateInfo::SetPolygonMode(VkPolygonMode mode)
    {
        rasterizer.polygonMode = mode;
        rasterizer.lineWidth   = 1.0f;
    }

    void RHIDynamicPipelineCreateInfo::SetCullMode(VkCullModeFlags cull_mode, VkFrontFace front_face)
    {
        rasterizer.cullMode  = cull_mode;
        rasterizer.frontFace = front_face;
    }

    void RHIDynamicPipelineCreateInfo::SetMultisamplingNone()
    {
        multisampling.sampleShadingEnable   = VK_FALSE;
        multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading      = 1.0f;
        multisampling.pSampleMask           = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable      = VK_FALSE;
    }

    void RHIDynamicPipelineCreateInfo::DisableBlending()
    {
        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable    = VK_FALSE;
    }

    void RHIDynamicPipelineCreateInfo::EnableBlendingAdditive()
    {
        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                                VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable         = VK_TRUE;
        color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
        color_blend_attachment.colorBlendOp        = VK_BLEND_OP_ADD;
        color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        color_blend_attachment.alphaBlendOp        = VK_BLEND_OP_ADD;
    }

    void RHIDynamicPipelineCreateInfo::EnableBlendingAlphablend()
    {
        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                                VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable         = VK_TRUE;
        color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
        color_blend_attachment.colorBlendOp        = VK_BLEND_OP_ADD;
        color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        color_blend_attachment.alphaBlendOp        = VK_BLEND_OP_ADD;
    }

    void RHIDynamicPipelineCreateInfo::SetColorAttachmentFormat(VkFormat format)
    {
        color_attachment_format = format;

        render_info.colorAttachmentCount    = 1;
        render_info.pColorAttachmentFormats = &color_attachment_format;
    }

    void RHIDynamicPipelineCreateInfo::SetDepthFormat(VkFormat format)
    {
        render_info.depthAttachmentFormat = format;
    }

    void RHIDynamicPipelineCreateInfo::DisableDepthTest()
    {
        depth_stencil.depthTestEnable       = VK_FALSE;
        depth_stencil.depthWriteEnable      = VK_FALSE;
        depth_stencil.depthCompareOp        = VK_COMPARE_OP_NEVER;
        depth_stencil.depthBoundsTestEnable = VK_FALSE;
        depth_stencil.stencilTestEnable     = VK_FALSE;
        depth_stencil.front                 = {};
        depth_stencil.back                  = {};
        depth_stencil.minDepthBounds        = 0.f;
        depth_stencil.maxDepthBounds        = 1.f;
    }

    void RHIDynamicPipelineCreateInfo::EnableDepthTest(bool depth_write_enable, VkCompareOp op)
    {
        depth_stencil.depthTestEnable       = VK_TRUE;
        depth_stencil.depthWriteEnable      = depth_write_enable;
        depth_stencil.depthCompareOp        = op;
        depth_stencil.depthBoundsTestEnable = VK_FALSE;
        depth_stencil.stencilTestEnable     = VK_FALSE;
        depth_stencil.front                 = {};
        depth_stencil.back                  = {};
        depth_stencil.minDepthBounds        = 0.0f;
        depth_stencil.maxDepthBounds        = 1.0f;
    }

} // namespace Yutrel