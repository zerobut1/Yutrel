#include "yutrel_pch.hpp"

#include "vulkan_utils.hpp"

#include "platform/Vulkan/rhi/vulkan_rhi.hpp"
#include <vulkan/vulkan_core.h>

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

    void DescriptorAllocator::Init(VulkanRHI* rhi, uint32_t initial_sets, std::vector<PoolSizeRatio>& pool_ratios)
    {
        m_rhi = rhi;

        m_ratios.clear();

        for (auto r : pool_ratios)
        {
            m_ratios.push_back(r);
        }

        VkDescriptorPool new_pool = CreatePool(initial_sets, pool_ratios);

        m_sets_per_pool = initial_sets * 1.5;

        m_ready_pools.push_back(new_pool);
    }

    void DescriptorAllocator::ClearPools()
    {
        for (auto p : m_ready_pools)
        {
            m_rhi->ResetDescriptorPool(p);
        }

        for (auto p : m_full_pools)
        {
            m_rhi->ResetDescriptorPool(p);
            m_ready_pools.push_back(p);
        }
        m_full_pools.clear();
    }

    void DescriptorAllocator::DestroyPools()
    {
        for (auto p : m_ready_pools)
        {
            m_rhi->DestroyDescriptorPool(p);
        }
        m_ready_pools.clear();
        for (auto p : m_full_pools)
        {
            m_rhi->DestroyDescriptorPool(p);
        }
        m_full_pools.clear();
    }

    VkDescriptorSet DescriptorAllocator::Allocate(VkDescriptorSetLayout layout)
    {
        VkDescriptorPool pool_to_use = GetPool();

        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.pNext              = nullptr;
        alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool     = pool_to_use;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts        = &layout;

        VkDescriptorSet descriptor_set;
        VkResult result = m_rhi->AllocateDescriptorSets(&alloc_info, &descriptor_set);

        // 若分配失败则重试
        if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL)
        {
            m_full_pools.push_back(pool_to_use);

            pool_to_use               = GetPool();
            alloc_info.descriptorPool = pool_to_use;

            YUTREL_ASSERT(m_rhi->AllocateDescriptorSets(&alloc_info, &descriptor_set) == VK_SUCCESS, "Failed to allocate descriptor sets");
        }

        m_ready_pools.push_back(pool_to_use);
        return descriptor_set;
    }

    VkDescriptorPool DescriptorAllocator::GetPool()
    {
        VkDescriptorPool new_pool;
        if (m_ready_pools.size() != 0)
        {
            new_pool = m_ready_pools.back();
            m_ready_pools.pop_back();
        }
        else
        {
            new_pool = CreatePool(m_sets_per_pool, m_ratios);

            m_sets_per_pool = m_sets_per_pool * 1.5;
            if (m_sets_per_pool > 4092)
            {
                m_sets_per_pool = 4092;
            }
        }

        return new_pool;
    }

    VkDescriptorPool DescriptorAllocator::CreatePool(uint32_t set_count, std::vector<PoolSizeRatio>& pool_ratios)
    {
        std::vector<VkDescriptorPoolSize> pool_sizes;
        for (PoolSizeRatio ratio : pool_ratios)
        {
            VkDescriptorPoolSize size{};
            size.type            = ratio.type;
            size.descriptorCount = static_cast<uint32_t>(ratio.ratio * set_count);
            pool_sizes.push_back(size);
        }

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags         = 0;
        pool_info.maxSets       = set_count;
        pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        pool_info.pPoolSizes    = pool_sizes.data();

        VkDescriptorPool new_pool;
        m_rhi->CreateDescriptorPool(&pool_info, &new_pool);

        return new_pool;
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