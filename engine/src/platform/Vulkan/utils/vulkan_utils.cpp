#include "yutrel_pch.hpp"

#include "vulkan_utils.hpp"

#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include <vulkan/vulkan_core.h>

namespace Yutrel
{
    void DynamicPipelineCreateInfo::Clear()
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

    void DynamicPipelineCreateInfo::SetShaders(VkShaderModule vertex_shader, VkShaderModule fragment_shader)
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

    void DynamicPipelineCreateInfo::SetInputTopolygy(VkPrimitiveTopology topology)
    {
        input_assembly.topology               = topology;
        input_assembly.primitiveRestartEnable = VK_FALSE;
    }

    void DynamicPipelineCreateInfo::SetPolygonMode(VkPolygonMode mode)
    {
        rasterizer.polygonMode = mode;
        rasterizer.lineWidth   = 1.0f;
    }

    void DynamicPipelineCreateInfo::SetCullMode(VkCullModeFlags cull_mode, VkFrontFace front_face)
    {
        rasterizer.cullMode  = cull_mode;
        rasterizer.frontFace = front_face;
    }

    void DynamicPipelineCreateInfo::SetMultisamplingNone()
    {
        multisampling.sampleShadingEnable   = VK_FALSE;
        multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading      = 1.0f;
        multisampling.pSampleMask           = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable      = VK_FALSE;
    }

    void DynamicPipelineCreateInfo::DisableBlending()
    {
        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable    = VK_FALSE;
    }

    void DynamicPipelineCreateInfo::EnableBlendingAdditive()
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

    void DynamicPipelineCreateInfo::EnableBlendingAlphablend()
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

    void DynamicPipelineCreateInfo::SetColorAttachmentFormat(VkFormat format)
    {
        color_attachment_format = format;

        render_info.colorAttachmentCount    = 1;
        render_info.pColorAttachmentFormats = &color_attachment_format;
    }

    void DynamicPipelineCreateInfo::SetDepthFormat(VkFormat format)
    {
        render_info.depthAttachmentFormat = format;
    }

    void DynamicPipelineCreateInfo::DisableDepthTest()
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

    void DynamicPipelineCreateInfo::EnableDepthTest(bool depth_write_enable, VkCompareOp op)
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

    void DescriptorWriter::WriteImage(int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type)
    {
        VkDescriptorImageInfo& info = image_infos.emplace_back(VkDescriptorImageInfo{sampler, image, layout});

        VkWriteDescriptorSet write{};
        write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstBinding      = binding;
        write.dstSet          = VK_NULL_HANDLE;
        write.descriptorCount = 1;
        write.descriptorType  = type;
        write.pImageInfo      = &info;

        writes.push_back(write);
    }

    void DescriptorWriter::WriteBuffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type)
    {
        VkDescriptorBufferInfo& info = buffer_infos.emplace_back(VkDescriptorBufferInfo{buffer, offset, size});

        VkWriteDescriptorSet write{};
        write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstBinding      = binding;
        write.dstSet          = VK_NULL_HANDLE;
        write.descriptorCount = 1;
        write.descriptorType  = type;
        write.pBufferInfo     = &info;

        writes.push_back(write);
    }

    void DescriptorWriter::Clear()
    {
        image_infos.clear();
        writes.clear();
        buffer_infos.clear();
    }

} // namespace Yutrel