#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/matrix.hpp>

#include <deque>
#include <string>
#include <vector>

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

    // 分配缓冲区
    struct AllocatedBuffer
    {
        VkBuffer buffer;
        VmaAllocation allocation;
        VmaAllocationInfo info;
    };

    // 分配图像
    struct AllocatedImage
    {
        VkImage image;
        VkImageView image_view;
        VmaAllocation allocation;
        VkExtent3D image_extent;
        VkFormat image_format;
    };

    // 帧数据
    struct FrameData
    {
        // 指令
        VkCommandPool command_pool;
        VkCommandBuffer main_command_buffer;

        // 同步
        VkSemaphore finished_for_presentation_semaphore, available_for_render_semaphore;
        VkFence render_fence;

        // 删除队列
        DeletionQueue deletion_queue;
    };

    // 交换链信息
    struct RHISwapChainDesc
    {
        VkExtent2D extent;
        VkFormat image_format;
        std::vector<VkImageView>* image_views;
    };

} // namespace Yutrel
