#include "initializers.hpp"

namespace Yutrel
{
    namespace vkinit
    {
        VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queue_family_index, VkCommandPoolCreateFlags flags)
        {
            VkCommandPoolCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            info.pNext = nullptr;

            info.queueFamilyIndex = queue_family_index;
            info.flags            = flags;

            return info;
        }

        VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool pool, uint32_t count, VkCommandBufferLevel level)
        {
            VkCommandBufferAllocateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            info.pNext = nullptr;

            info.commandPool        = pool;
            info.commandBufferCount = count;
            info.level              = level;

            return info;
        }

        VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags)
        {
            VkFenceCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            info.pNext = nullptr;

            info.flags = flags;

            return info;
        }

        VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags)
        {
            VkSemaphoreCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            info.pNext = nullptr;

            info.flags = flags;

            return info;
        }
    } // namespace vkinit
} // namespace Yutrel