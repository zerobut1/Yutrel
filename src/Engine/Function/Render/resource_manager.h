#pragma once

#include <vk_mem_alloc.hpp>

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
        vma::Allocation allocation;
        vma::AllocationInfo info;
    };

    // 图像
    struct Image
    {
        vk::Image image;
        vk::ImageView image_view;
        vk::Extent3D extent;
        vk::Format format;
        vma::Allocation allocation;
    };

    class ResourceManager final
    {
    public:
        ResourceManager() = delete;
        explicit ResourceManager(std::shared_ptr<class Context> context);
        ~ResourceManager();

        ResourceManager(const ResourceManager&)            = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        Image CreateImage();

    private:
        void init();
        void destroy();

    private:
        std::shared_ptr<Context> m_context;

        vma::Allocator m_allocator{nullptr};
    };
} // namespace Yutrel