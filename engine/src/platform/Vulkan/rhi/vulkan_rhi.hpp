#pragma once

#include "function/render/rhi.hpp"
#include "platform/Vulkan/vulkan_types.hpp"

#include <deque>
#include <functional>
#include <stdint.h>

struct GLFWwindow;

namespace Yutrel
{
    class VulkanRHI : public RHI
    {
    public:
        virtual void Init(RHIInitInfo info) override;

        virtual void Clear() override;

    private:
        //--------初始化----------
        void InitVulkan(GLFWwindow* raw_window, uint32_t width, uint32_t height);
        // 交换链
        void InitSwapchain();
        // renderpass
        void InitDefaultRenderPass();
        // 帧缓冲
        void InitFramebuffers();
        // 指令池与指令缓冲
        void InitCommands();
        // 同步设施
        void InitSyncStructures();
        // 描述符
        void InitDescriptors();
        //------------------------

    public:
        // 内存分配器
        VmaAllocator allocator;

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
        DeletionQueue main_deletion_queue;

    private:
        bool m_enable_validation_layers{true};

        // vulkan交换范围
        VkExtent2D m_window_extent;

        // vulkan实例
        VkInstance m_instance;
        // vulkan DebugMessenger
        VkDebugUtilsMessengerEXT m_debug_messenger;
        // 物理设备
        VkPhysicalDevice m_physical_device;
        // 物理设备信息
        VkPhysicalDeviceProperties m_physical_device_properties;
        // 逻辑设备
        VkDevice m_device;
        // 窗口表面
        VkSurfaceKHR m_surface;
        // 队列
        VkQueue m_graphics_queue;
        uint32_t m_graphics_queue_family;

        // 交换链
        VkSwapchainKHR m_swapchain;
        VkFormat m_swapchain_image_format;
        // 交换链图像
        std::vector<VkImage> m_swapchain_images;
        std::vector<VkImageView> m_swapchain_image_views;
    };
} // namespace Yutrel