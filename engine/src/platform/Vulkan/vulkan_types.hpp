#pragma once

#include <vk_mem_alloc.hpp>
#include <vk_mem_alloc_handles.hpp>
#include <vk_mem_alloc_structs.hpp>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.hpp>

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
        vk::Buffer buffer;
        vma::Allocation allocation;
        vma::AllocationInfo info;
    };

    // 分配图像
    struct AllocatedImage
    {
        vk::Image image;
        vk::ImageView image_view;
        vk::Extent3D extent;
        vk::Format format;
        vma::Allocation allocation;
        bool mipmapped{false};
    };

} // namespace Yutrel
