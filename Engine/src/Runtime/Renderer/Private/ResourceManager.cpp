#include "ResourceManager.h"

#include "Context.h"
#include "DescriptorSet.h"
#include "Log.h"
#include "Pipeline.h"

namespace Yutrel
{
    ResourceManager::ResourceManager(std::shared_ptr<class Context> context)
        : m_context(context)
    {
        init();
    }

    ResourceManager::~ResourceManager()
    {
        destroy();
    }

    void ResourceManager::init()
    {
        // vma
        auto allocator_ci             = VmaAllocatorCreateInfo{};
        allocator_ci.flags            = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
        allocator_ci.vulkanApiVersion = vk::ApiVersion13;
        allocator_ci.physicalDevice   = m_context->getGPU();
        allocator_ci.device           = m_context->getDevice();
        allocator_ci.instance         = m_context->getInstance();

        vmaCreateAllocator(&allocator_ci, &m_allocator);
    }

    void ResourceManager::destroy()
    {
        m_main_deletion_queue.flush();
        vmaDestroyAllocator(m_allocator);
    }

    Buffer ResourceManager::createBuffer(const vk::BufferCreateInfo& create_info, const VmaAllocationCreateInfo& allocation_info)
    {
        Buffer new_buffer;
        vmaCreateBuffer(m_allocator, (VkBufferCreateInfo*)&create_info, &allocation_info, (VkBuffer*)&new_buffer.buffer, &new_buffer.allocation, &new_buffer.info);

        m_main_deletion_queue.PushFunction(
            [=, this]()
            {
                vmaDestroyBuffer(m_allocator, static_cast<VkBuffer>(new_buffer.buffer), new_buffer.allocation);
            });

        return new_buffer;
    }

    Image ResourceManager::createImage(const vk::ImageCreateInfo& info)
    {
        Image res_image;

        auto image_ai          = VmaAllocationCreateInfo{};
        image_ai.usage         = VMA_MEMORY_USAGE_AUTO;
        image_ai.requiredFlags = VkMemoryAllocateFlags(vk::MemoryPropertyFlagBits::eDeviceLocal);

        vmaCreateImage(m_allocator, (VkImageCreateInfo*)&info, &image_ai, (VkImage*)&res_image.image, &res_image.allocation, &res_image.info);

        m_main_deletion_queue.PushFunction(
            [=, this]()
            {
                vmaDestroyImage(m_allocator, static_cast<VkImage>(res_image.image), res_image.allocation);
            });

        return res_image;
    }

    vk::ImageView ResourceManager::createImageView(const vk::ImageViewCreateInfo& info)
    {
        auto device = m_context->getDevice();

        auto res_image_view = device.createImageView(info);

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                device.destroyImageView(res_image_view);
            });

        return res_image_view;
    }

    vk::Sampler ResourceManager::createSampler(const vk::SamplerCreateInfo& info)
    {
        auto device = m_context->getDevice();

        auto res_sampler = device.createSampler(info);

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                device.destroy(res_sampler);
            });

        return res_sampler;
    }

    vk::DescriptorSetLayout ResourceManager::createDescriptorSetLayout(DescriptorSetLayoutCreateInfo& info)
    {
        auto device = m_context->getDevice();

        for (auto& b : info.bindings)
        {
            b.stageFlags |= info.shader_stages;
        }

        auto layout_ci =
            vk::DescriptorSetLayoutCreateInfo()
                .setBindings(info.bindings)
                .setFlags({});

        vk::DescriptorSetLayout layout = device.createDescriptorSetLayout(layout_ci);

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                device.destroyDescriptorSetLayout(layout);
            });

        return layout;
    }

    vk::PipelineLayout ResourceManager::createPipelineLayout(const vk::PipelineLayoutCreateInfo& info)
    {
        auto device = m_context->getDevice();

        vk::PipelineLayout layout = device.createPipelineLayout(info);

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                device.destroyPipelineLayout(layout);
            });

        return layout;
    }

    vk::Pipeline ResourceManager::createRenderPipeline(const RenderPipelineCreateInfo& info)
    {
        auto device = m_context->getDevice();

        // 视口和裁剪
        auto viewport_state_ci =
            vk::PipelineViewportStateCreateInfo()
                .setViewportCount(1)
                .setScissorCount(1);

        // 颜色混合
        std::vector<vk::PipelineColorBlendAttachmentState> color_blend_attachments;
        for (size_t i = 0; i < info.render_info.colorAttachmentCount; i++)
        {
            color_blend_attachments.push_back(info.color_blend_attachment);
        }

        auto color_blend_state_ci =
            vk::PipelineColorBlendStateCreateInfo()
                .setLogicOpEnable(vk::False)
                .setLogicOp(vk::LogicOp::eCopy)
                .setAttachments(color_blend_attachments);

        // 顶点阶段
        // 不需要
        auto vertex_input_state_ci = vk::PipelineVertexInputStateCreateInfo();

        // 动态状态
        std::array<vk::DynamicState, 2> states =
            {
                vk::DynamicState::eViewport,
                vk::DynamicState::eScissor,
            };
        auto dynamic_state_ci =
            vk::PipelineDynamicStateCreateInfo()
                .setDynamicStates(states);

        //---------管线创建信息-------------
        auto pipeline_ci =
            vk::GraphicsPipelineCreateInfo()
                .setPNext(&info.render_info)
                .setStages(info.shader_stages)
                .setPVertexInputState(&vertex_input_state_ci)
                .setPInputAssemblyState(&info.input_assembly)
                .setPViewportState(&viewport_state_ci)
                .setPRasterizationState(&info.rasterizer)
                .setPMultisampleState(&info.multisampling)
                .setPColorBlendState(&color_blend_state_ci)
                .setPDepthStencilState(&info.depth_stencil)
                .setPDynamicState(&dynamic_state_ci)
                .setLayout(info.pipeline_layout);

        auto result = device.createGraphicsPipeline(nullptr, pipeline_ci);
        YUTREL_ASSERT(result.result == vk::Result::eSuccess, "Failed to create pipelines");

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                device.destroyPipeline(result.value);
            });

        return result.value;
    }

    vk::Pipeline ResourceManager::createComputePipeline(const vk::ComputePipelineCreateInfo& info)
    {
        auto device = m_context->getDevice();

        auto result = device.createComputePipeline(nullptr, info);
        YUTREL_ASSERT(result.result == vk::Result::eSuccess, "Failed to create pipelines");

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                device.destroyPipeline(result.value);
            });

        return result.value;
    }

} // namespace Yutrel