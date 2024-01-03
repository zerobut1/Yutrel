#pragma once

#include "platform/Vulkan/vulkan_types.hpp"

namespace Yutrel
{
    // 图形管线创建信息
    struct RHIGraphicsPipelineCreateInfo
    {
        // 着色器阶段
        std::vector<VkPipelineShaderStageCreateInfo> shader_stages;

        // 渲染管线布局
        VkPipelineLayout pipeline_layout;

        // 图元装配
        VkPipelineInputAssemblyStateCreateInfo input_assembly;

        // 光栅化阶段
        VkPipelineRasterizationStateCreateInfo rasterizer;

        // MSAA
        VkPipelineMultisampleStateCreateInfo multisampling;

        // 颜色混合
        VkPipelineColorBlendStateCreateInfo color_blend;

        // 顶点输入阶段
        VkPipelineVertexInputStateCreateInfo vertex_input;

        // 视口阶段
        VkPipelineViewportStateCreateInfo viewport_state;

        // 渲染pass
        VkRenderPass render_pass;

        // subpass
        uint32_t subpass;
    };

    // 动态渲染管线创建信息
    struct RHIDynamicPipelineCreateInfo
    {
        // 着色器阶段
        std::vector<VkPipelineShaderStageCreateInfo> shader_stages;

        // 渲染管线布局
        VkPipelineLayout pipeline_layout;

        // 图元装配
        VkPipelineInputAssemblyStateCreateInfo input_assembly;

        // 光栅化阶段
        VkPipelineRasterizationStateCreateInfo rasterizer;

        // MSAA
        VkPipelineMultisampleStateCreateInfo multisampling;

        // 颜色混合
        VkPipelineColorBlendAttachmentState color_blend_attachment;

        // 深度模板阶段
        VkPipelineDepthStencilStateCreateInfo depth_stencil;

        // 渲染创建信息
        VkPipelineRenderingCreateInfo render_info;

        // 颜色缓冲格式
        VkFormat color_attachment_format;

        void Clear();

        void SetShaders(VkShaderModule vertex_shader, VkShaderModule fragment_shader);

        void SetInputTopolygy(VkPrimitiveTopology topology);

        void SetPolygonMode(VkPolygonMode mode);

        void SetCullMode(VkCullModeFlags cull_mode, VkFrontFace front_face);

        void SetMultisamplingNone();

        void DisableBlending();

        void EnableBlendingAdditive();

        void EnableBlendingAlphablend();

        void SetColorAttachmentFormat(VkFormat format);

        void SetDepthFormat(VkFormat format);

        void DisableDepthTest();

        void EnableDepthTest(bool depth_write_enable, VkCompareOp op);
    };

    struct RHIDescriptorLayoutCreateInfo
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        VkShaderStageFlags shader_stages;

        void AddBinding(uint32_t binding, VkDescriptorType type)
        {
            VkDescriptorSetLayoutBinding newbind{};
            newbind.binding         = binding;
            newbind.descriptorCount = 1;
            newbind.descriptorType  = type;

            bindings.push_back(newbind);
        }

        void Clear()
        {
            bindings.clear();
        }
    };
} // namespace Yutrel