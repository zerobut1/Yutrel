#pragma once

#include <vulkan/vulkan.hpp>

namespace Yutrel
{
    struct RenderPipelineCreateInfo
    {
        // 着色器阶段
        std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;

        // 渲染管线布局
        vk::PipelineLayout pipeline_layout{};

        // 图元装配
        vk::PipelineInputAssemblyStateCreateInfo input_assembly{};

        // 光栅化阶段
        vk::PipelineRasterizationStateCreateInfo rasterizer{};

        // MSAA
        vk::PipelineMultisampleStateCreateInfo multisampling{};

        // 颜色混合
        vk::PipelineColorBlendAttachmentState color_blend_attachment{};

        // 深度模板阶段
        vk::PipelineDepthStencilStateCreateInfo depth_stencil{};

        // 渲染创建信息
        vk::PipelineRenderingCreateInfo render_info{};

        // 颜色缓冲格式
        vk::Format color_attachment_format{};

        RenderPipelineCreateInfo();

        RenderPipelineCreateInfo& SetPipelineLayout(vk::PipelineLayout layout);

        RenderPipelineCreateInfo& SetShaders(vk::ShaderModule vertex_shader, vk::ShaderModule fragment_shader);

        RenderPipelineCreateInfo& SetComputeShader(vk::ShaderModule shader);

        RenderPipelineCreateInfo& SetInputTopolygy(vk::PrimitiveTopology topology);

        RenderPipelineCreateInfo& SetPolygonMode(vk::PolygonMode mode);

        RenderPipelineCreateInfo& SetCullMode(vk::CullModeFlags cull_mode, vk::FrontFace front_face);

        RenderPipelineCreateInfo& SetMultisamplingNone();

        RenderPipelineCreateInfo& DisableBlending();

        RenderPipelineCreateInfo& EnableBlendingAdditive();

        RenderPipelineCreateInfo& EnableBlendingAlphablend();

        RenderPipelineCreateInfo& SetColorAttachmentFormat(vk::Format format);

        RenderPipelineCreateInfo& SetColorAttachmentFormats(std::vector<vk::Format> formats);

        RenderPipelineCreateInfo& SetDepthFormat(vk::Format format);

        RenderPipelineCreateInfo& DisableDepthTest();

        RenderPipelineCreateInfo& EnableDepthTest(bool depth_write_enable, vk::CompareOp op);
    };
} // namespace Yutrel