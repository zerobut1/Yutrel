#include "ResourceManager.h"

#include "Context.h"

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

    Image ResourceManager::createImage(const vk::ImageCreateInfo& info)
    {
        Image res_image;

        auto image_ai          = VmaAllocationCreateInfo{};
        image_ai.usage         = VMA_MEMORY_USAGE_AUTO;
        image_ai.requiredFlags = VkMemoryAllocateFlags(vk::MemoryPropertyFlagBits::eDeviceLocal);

        vmaCreateImage(m_allocator, (VkImageCreateInfo*)&info, &image_ai, (VkImage*)&res_image.image, &res_image.allocation, nullptr);

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vmaDestroyImage(m_allocator, res_image.image, res_image.allocation);
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

} // namespace Yutrel