#include "yutrel_pch.hpp"

#include "vulkan_utils.hpp"

#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

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
                               vk::ColorComponentFlagBits::eB |
                               vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::True)
            .setSrcColorBlendFactor(vk::BlendFactor::eOneMinusDstAlpha)
            .setDstColorBlendFactor(vk::BlendFactor::eDstAlpha)
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

    DescriptorSetLayoutCreateInfo& DescriptorSetLayoutCreateInfo::AddBinding(uint32_t binding, vk::DescriptorType type)
    {
        auto new_bind =
            vk::DescriptorSetLayoutBinding()
                .setBinding(binding)
                .setDescriptorCount(1)
                .setDescriptorType(type);

        bindings.push_back(new_bind);

        return *this;
    }

    DescriptorSetLayoutCreateInfo& DescriptorSetLayoutCreateInfo::SetShaderStage(vk::ShaderStageFlags shader_stage)
    {
        shader_stages = shader_stage;

        return *this;
    }

    void DescriptorSetLayoutCreateInfo::Clear()
    {
        bindings.clear();
    }

    DescriptorWriter& DescriptorWriter::WriteImage(int binding, vk::ImageView image, vk::Sampler sampler, vk::ImageLayout layout, vk::DescriptorType type)
    {
        auto& info =
            image_infos.emplace_back(
                vk::DescriptorImageInfo()
                    .setSampler(sampler)
                    .setImageView(image)
                    .setImageLayout(layout));

        auto write =
            vk::WriteDescriptorSet()
                .setDstSet({})
                .setDstBinding(binding)
                .setDescriptorCount(1)
                .setDescriptorType(type)
                .setImageInfo(info);

        writes.push_back(write);

        return *this;
    }

    DescriptorWriter& DescriptorWriter::WriteBuffer(int binding, vk::Buffer buffer, size_t size, size_t offset, vk::DescriptorType type)
    {
        auto& info =
            buffer_infos.emplace_back(
                vk::DescriptorBufferInfo()
                    .setBuffer(buffer)
                    .setOffset(offset)
                    .setRange(size));

        auto write =
            vk::WriteDescriptorSet()
                .setDstSet({})
                .setDstBinding(binding)
                .setDescriptorCount(1)
                .setDescriptorType(type)
                .setBufferInfo(info);

        writes.push_back(write);

        return *this;
    }

    void DescriptorWriter::Clear()
    {
        image_infos.clear();
        buffer_infos.clear();
        writes.clear();
    }

} // namespace Yutrel