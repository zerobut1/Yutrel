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
    } // namespace vkinit
} // namespace Yutrel