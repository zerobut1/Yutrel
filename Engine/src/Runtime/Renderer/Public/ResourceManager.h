#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include <deque>
#include <functional>

namespace Yutrel
{
    // 删除队列
    struct DeletionQueue
    {
        std::deque<std::function<void()>> deletors;

        void PushFunction(std::function<void()>&& function)
        {
            deletors.push_back(function);
        }

        void flush()
        {
            for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
            {
                // 调用删除函数
                (*it)();
            }

            deletors.clear();
        }
    };

    // 缓冲区
    struct Buffer
    {
        vk::Buffer buffer;
        VmaAllocation allocation;
        VmaAllocationInfo info;
    };

    // 图像
    struct Image
    {
        vk::Image image;
        VmaAllocation allocation;
        VmaAllocationInfo info;
    };

    class ResourceManager final
    {
    public:
        ResourceManager() = delete;
        explicit ResourceManager(std::shared_ptr<class Context> context);
        ~ResourceManager();

        ResourceManager(const ResourceManager&)            = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

    public:
        Buffer createBuffer(const vk::BufferCreateInfo& create_info, const VmaAllocationCreateInfo& allocation_info);

        Image createImage(const vk::ImageCreateInfo& info);
        vk::ImageView createImageView(const vk::ImageViewCreateInfo& info);
        vk::Sampler createSampler(const vk::SamplerCreateInfo& info);

        vk::DescriptorSetLayout createDescriptorSetLayout(struct DescriptorSetLayoutCreateInfo& info);

        vk::PipelineLayout createPipelineLayout(const vk::PipelineLayoutCreateInfo& info);

        vk::Pipeline createRenderPipeline(const struct RenderPipelineCreateInfo& info);

        vk::Pipeline createComputePipeline(const vk::ComputePipelineCreateInfo& info);

    private:
        void init();
        void destroy();

    private:
        std::shared_ptr<Context> m_context;

        VmaAllocator m_allocator{nullptr};
        DeletionQueue m_main_deletion_queue;
    };
} // namespace Yutrel