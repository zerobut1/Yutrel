#include "Renderer.h"

#include "Context.h"
#include "DescriptorSet.h"
#include "Frame.h"
#include "Log.h"
#include "Pipeline.h"
#include "ResourceManager.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace Yutrel
{
    Renderer::Renderer(const CreateInfo& info)
    {
        LOG_INFO("Create Vulkan Renderer");
        init(info);
    }

    Renderer::~Renderer()
    {
        shutdown();
    }

    void Renderer::init(const CreateInfo& info)
    {
        //--------context------------
        Context::CreateInfo context_ci{};
        context_ci.device_features    = info.device_features;
        context_ci.device_features_12 = info.device_features_12;
        context_ci.device_features_13 = info.device_features_13;

        m_context = std::make_shared<Context>(context_ci);

        //----------帧数据------------
        for (auto& frame : m_frames)
        {
            frame = std::make_shared<Frame>(m_context);
        }

        //--------资源管理—————————————
        m_resource_manager = std::make_shared<ResourceManager>(m_context);

        //---------单次指令池-------------
        auto cmd_pool_ci =
            vk::CommandPoolCreateInfo()
                .setQueueFamilyIndex(m_context->getGraphicsQueueIndex())
                .setFlags({});

        m_cmd_pool = m_context->getDevice().createCommandPool(cmd_pool_ci);

        //--------descriptorsets---------
        std::vector<vk::DescriptorPoolSize> sizes{
            {vk::DescriptorType::eUniformBuffer, 100},
            {vk::DescriptorType::eStorageImage, 100},
            {vk::DescriptorType::eCombinedImageSampler, 100},
        };

        auto pool_ci =
            vk::DescriptorPoolCreateInfo()
                .setFlags({})
                .setMaxSets(100)
                .setPoolSizes(sizes);

        m_descriptor_pool = m_context->getDevice().createDescriptorPool(pool_ci);
    }

    void Renderer::shutdown()
    {
        auto device = m_context->getDevice();

        device.waitIdle();

        device.destroy(m_descriptor_pool);

        device.destroy(m_cmd_pool);

        m_resource_manager.reset();

        for (auto frame : m_frames)
        {
            frame.reset();
        }

        m_context.reset();
    }

    std::shared_ptr<Frame> Renderer::prepareBeforeRender()
    {
        auto cur_frame = getCurrentFrame();

        // fence
        cur_frame->waitForFence();

        return cur_frame;
    }

    void Renderer::submitRendering(std::shared_ptr<Frame> cur_frame)
    {
        // 终止指令缓冲
        cur_frame->endCommandBuffer();

        // 提交指令
        cur_frame->submitCommandBuffer();

        m_frame_count++;
    }

    vk::CommandBuffer Renderer::beginSingleTimeCommandBuffer()
    {
        auto cmd_buffer_ai =
            vk::CommandBufferAllocateInfo()
                .setCommandPool(m_cmd_pool)
                .setCommandBufferCount(1)
                .setLevel(vk::CommandBufferLevel::ePrimary);

        vk::CommandBuffer cmd_buffer = m_context->getDevice().allocateCommandBuffers(cmd_buffer_ai).front();

        auto cmd_buffer_bi =
            vk::CommandBufferBeginInfo()
                .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

        cmd_buffer.begin(cmd_buffer_bi);

        return cmd_buffer;
    }

    void Renderer::endSingleTimeCommandBuffer(vk::CommandBuffer cmd_buffer)
    {
        cmd_buffer.end();

        auto cmd_buffer_si =
            vk::CommandBufferSubmitInfo()
                .setCommandBuffer(cmd_buffer);
        auto submit_info =
            vk::SubmitInfo2()
                .setCommandBufferInfos(cmd_buffer_si);

        auto queue = m_context->getGraphicsQueue();

        queue.submit2(submit_info);
        queue.waitIdle();

        m_context->getDevice().freeCommandBuffers(m_cmd_pool, cmd_buffer);
    }

    Buffer Renderer::createBuffer(size_t alloc_size, vk::BufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage, VkMemoryPropertyFlags required_flags)
    {
        auto buffer_ci =
            vk::BufferCreateInfo()
                .setSize(alloc_size)
                .setUsage(buffer_usage);

        auto allocation_ci  = VmaAllocationCreateInfo{};
        allocation_ci.usage = memory_usage;
        allocation_ci.flags = required_flags;

        return m_resource_manager->createBuffer(buffer_ci, allocation_ci);
    }

    void Renderer::transitionImageLayout(vk::CommandBuffer cmd_buffer, vk::Image image, vk::ImageLayout cur_layout, vk::ImageLayout new_layout)
    {
        vk::ImageAspectFlags aspect_mask = vk::ImageAspectFlagBits::eColor;

        auto image_barrier =
            vk::ImageMemoryBarrier2()
                .setSrcStageMask(vk::PipelineStageFlagBits2::eAllCommands)
                .setSrcAccessMask(vk::AccessFlagBits2::eMemoryWrite)
                .setDstStageMask(vk::PipelineStageFlagBits2::eAllCommands)
                .setDstAccessMask(vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead)
                .setOldLayout(cur_layout)
                .setNewLayout(new_layout)
                .setSubresourceRange(vk::ImageSubresourceRange(aspect_mask, 0, vk::RemainingMipLevels, 0, vk::RemainingArrayLayers))
                .setImage(image);

        auto dependency_info =
            vk::DependencyInfo()
                .setImageMemoryBarriers(image_barrier);

        cmd_buffer.pipelineBarrier2(dependency_info);
    }

    void Renderer::copyImageToImage(vk::CommandBuffer cmd_buffer, vk::Image source, vk::Image destination, vk::Extent2D src_size, vk::Extent2D dst_size)
    {
        auto blit_region =
            vk::ImageBlit2()
                .setSrcOffsets({vk::Offset3D(),
                                vk::Offset3D(src_size.width, src_size.height, 1)})
                .setDstOffsets({vk::Offset3D(),
                                vk::Offset3D(dst_size.width, dst_size.height, 1)})
                .setSrcSubresource(vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1))
                .setDstSubresource(vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1));

        auto blit_image_info =
            vk::BlitImageInfo2()
                .setSrcImage(source)
                .setSrcImageLayout(vk::ImageLayout::eTransferSrcOptimal)
                .setDstImage(destination)
                .setDstImageLayout(vk::ImageLayout::eTransferDstOptimal)
                .setFilter(vk::Filter::eLinear)
                .setRegions(blit_region);

        cmd_buffer.blitImage2(blit_image_info);
    }

    vk::DescriptorSetLayout Renderer::createDescriptorSetLayout(DescriptorSetLayoutCreateInfo& info)
    {
        return m_resource_manager->createDescriptorSetLayout(info);
    }

    vk::DescriptorSet Renderer::allocateDescriptorSets(vk::DescriptorSetLayout layout)
    {
        auto set_ai =
            vk::DescriptorSetAllocateInfo()
                .setDescriptorPool(m_descriptor_pool)
                .setDescriptorSetCount(1)
                .setSetLayouts(layout);

        vk::DescriptorSet set = m_context->getDevice().allocateDescriptorSets(set_ai).front();

        return set;
    }

    void Renderer::updateDescriptorSets(DescriptorWriter& writer, vk::DescriptorSet set)
    {
        for (auto& write : writer.writes)
        {
            write.setDstSet(set);
        }
        m_context->getDevice().updateDescriptorSets(writer.writes, {});
    }

    vk::ShaderModule Renderer::createShaderModule(const std::vector<unsigned char>& shader_code)
    {
        auto shader_ci =
            vk::ShaderModuleCreateInfo()
                .setCodeSize(shader_code.size())
                .setPCode(reinterpret_cast<const uint32_t*>(shader_code.data()));

        VkShaderModule shader_module = m_context->getDevice().createShaderModule(shader_ci);

        return shader_module;
    }

    void Renderer::destroyShaderModule(vk::ShaderModule shader)
    {
        m_context->getDevice().destroyShaderModule(shader);
    }

    vk::PipelineLayout Renderer::createPipelineLayout(const vk::PipelineLayoutCreateInfo& info)
    {
        return m_resource_manager->createPipelineLayout(info);
    }

    vk::Pipeline Renderer::createRenderPipeline(const RenderPipelineCreateInfo& info)
    {
        return m_resource_manager->createRenderPipeline(info);
    }

    vk::Pipeline Renderer::createComputePipeline(const vk::ComputePipelineCreateInfo& info)
    {
        return m_resource_manager->createComputePipeline(info);
    }

} // namespace Yutrel