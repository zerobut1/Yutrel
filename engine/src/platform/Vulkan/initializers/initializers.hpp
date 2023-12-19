#pragma once

#include "platform/Vulkan/vulkan_types.hpp"

namespace Yutrel
{
    namespace vkinit
    {
        // 指令池创建信息
        VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queue_family_index, VkCommandPoolCreateFlags flags);

        // 指令缓冲分配信息
        VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool pool, uint32_t count, VkCommandBufferLevel level);
    } // namespace vkinit

} // namespace Yutrel