#include "Pipeline.h"

namespace Yutrel
{
    RenderPipelineCreateInfo::RenderPipelineCreateInfo()
    {
        shader_stages.clear();
    }

    RenderPipelineCreateInfo& RenderPipelineCreateInfo::SetPipelineLayout(vk::PipelineLayout layout)
    {
        pipeline_layout = layout;

        return *this;
    }

    RenderPipelineCreateInfo& RenderPipelineCreateInfo::SetShaders(vk::ShaderModule vertex_shader, vk::ShaderModule fragment_shader)
    {
        shader_stages.clear();

        // 顶点着色器
        auto shader_stage_ci =
            vk::PipelineShaderStageCreateInfo()
                .setStage(vk::ShaderStageFlagBits::eVertex)
                .setModule(vertex_shader)
                .setPName("main");

        shader_stages.push_back(shader_stage_ci);

        // 片段着色器
        shader_stage_ci
            .setStage(vk::ShaderStageFlagBits::eFragment)
            .setModule(fragment_shader);
        shader_stages.push_back(shader_stage_ci);

        return *this;
    }

    RenderPipelineCreateInfo& RenderPipelineCreateInfo::SetComputeShader(vk::ShaderModule shader)
    {
        shader_stages.clear();

        auto shader_stage_ci =
            vk::PipelineShaderStageCreateInfo()
                .setStage(vk::ShaderStageFlagBits::eCompute)
                .setModule(shader)
                .setPName("main");

        shader_stages.push_back(shader_stage_ci);

        return *this;
    }

    RenderPipelineCreateInfo& RenderPipelineCreateInfo::SetInputTopolygy(vk::PrimitiveTopology topology)
    {
        input_assembly
            .setTopology(topology)
            .setPrimitiveRestartEnable(vk::False);

        return *this;
    }

    RenderPipelineCreateInfo& RenderPipelineCreateInfo::SetPolygonMode(vk::PolygonMode mode)
    {
        rasterizer
            .setPolygonMode(mode)
            .setLineWidth(1.0f);

        return *this;
    }

    RenderPipelineCreateInfo& RenderPipelineCreateInfo::SetCullMode(vk::CullModeFlags cull_mode, vk::FrontFace front_face)
    {
        rasterizer
            .setCullMode(cull_mode)
            .setFrontFace(front_face);

        return *this;
    }

    RenderPipelineCreateInfo& RenderPipelineCreateInfo::SetMultisamplingNone()
    {
        multisampling
            .setSampleShadingEnable(vk::False)
            .setRasterizationSamples(vk::SampleCountFlagBits::e1)
            .setMinSampleShading(1.0f)
            .setPSampleMask(nullptr)
            .setAlphaToCoverageEnable(vk::False)
            .setAlphaToOneEnable(vk::False);

        return *this;
    }

    RenderPipelineCreateInfo& RenderPipelineCreateInfo::DisableBlending()
    {
        color_blend_attachment
            .setColorWriteMask(vk::ColorComponentFlagBits::eR |
                               vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB |
                               vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False);

        return *this;
    }

    RenderPipelineCreateInfo& RenderPipelineCreateInfo::EnableBlendingAdditive()
    {
        color_blend_attachment
            .setColorWriteMask(vk::ColorComponentFlagBits::eR |
                               vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB |
                               vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::True)
            .setSrcColorBlendFactor(vk::BlendFactor::eOne)
            .setDstColorBlendFactor(vk::BlendFactor::eDstAlpha)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd);

        return *this;
    }

    RenderPipelineCreateInfo& RenderPipelineCreateInfo::EnableBlendingAlphablend()
    {
        color_blend_attachment
            .setColorWriteMask(vk::ColorComponentFlagBits::eR |
                               vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB)
            .setBlendEnable(vk::True)
            .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
            .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd);

        return *this;
    }

    RenderPipelineCreateInfo& RenderPipelineCreateInfo::SetColorAttachmentFormat(vk::Format format)
    {
        color_attachment_format = format;

        render_info.setColorAttachmentFormats(color_attachment_format);

        return *this;
    }

    RenderPipelineCreateInfo& RenderPipelineCreateInfo::SetColorAttachmentFormats(std::vector<vk::Format> formats)
    {
        render_info.setColorAttachmentFormats(formats);

        return *this;
    }

    RenderPipelineCreateInfo& RenderPipelineCreateInfo::SetDepthFormat(vk::Format format)
    {
        render_info.setDepthAttachmentFormat(format);

        return *this;
    }

    RenderPipelineCreateInfo& RenderPipelineCreateInfo::DisableDepthTest()
    {
        depth_stencil
            .setDepthTestEnable(vk::False)
            .setDepthWriteEnable(vk::False)
            .setDepthCompareOp(vk::CompareOp::eNever)
            .setDepthBoundsTestEnable(vk::False)
            .setStencilTestEnable(vk::False)
            .setFront({})
            .setBack({})
            .setMinDepthBounds(0.0f)
            .setMaxDepthBounds(1.0f);

        return *this;
    }

    RenderPipelineCreateInfo& RenderPipelineCreateInfo::EnableDepthTest(bool depth_write_enable, vk::CompareOp op)
    {
        depth_stencil
            .setDepthTestEnable(vk::True)
            .setDepthWriteEnable(depth_write_enable)
            .setDepthCompareOp(op)
            .setDepthBoundsTestEnable(vk::False)
            .setStencilTestEnable(vk::False)
            .setFront({})
            .setBack({})
            .setMinDepthBounds(0.0f)
            .setMaxDepthBounds(1.0f);

        return *this;
    }
} // namespace Yutrel