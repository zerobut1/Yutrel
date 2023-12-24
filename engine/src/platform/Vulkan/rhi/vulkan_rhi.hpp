#pragma once

#include "platform/Vulkan/vulkan_types.hpp"

#include <array>
#include <deque>
#include <functional>
#include <stdint.h>

struct GLFWwindow;

namespace Yutrel
{
    struct RHIInitInfo
    {
        GLFWwindow* raw_window;
        uint32_t width;
        uint32_t height;
    };

    class VulkanRHI
    {
    public:
        void Init(RHIInitInfo info);

        void Clear();

        //---------获取信息------------
        RHISwapChainDesc GetSwapChainInfo();

        //---------创建对象------------
        bool CreateRenderPass(const VkRenderPassCreateInfo& info, VkRenderPass* render_pass);
        bool CreateFramebuffer(const VkFramebufferCreateInfo& info, VkFramebuffer* framebuffer);

    private:
        // 获取当前帧
        FrameData& GetCurrentFrame()
        {
            return m_frames[m_cur_frame % FRAME_OVERLAP];
        }

        //--------初始化----------
        void InitVulkan(GLFWwindow* raw_window, uint32_t width, uint32_t height);
        // 交换链
        void InitSwapchain();
        // 指令池与指令缓冲
        void InitCommands();
        // 描述符
        void InitDescriptorPool();
        // 同步设施
        void InitSyncStructures();
        // 帧缓冲
        void InitDepthImage();
        //------------------------

    private:
        // 同时渲染的帧数
        static constexpr uint8_t FRAME_OVERLAP{2};

        // 允许验证层
        bool m_enable_validation_layers{true};

        // 当前帧数
        uint32_t m_cur_frame{0};

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
        DeletionQueue m_main_deletion_queue;

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
        // 内存分配器
        VmaAllocator m_allocator;

        // 单次指令的指令池
        VkCommandPool m_rhi_command_pool;

        // 帧数据
        std::array<FrameData, FRAME_OVERLAP> m_frames;

        // 描述符池
        VkDescriptorPool m_descriptor_pool;

        // 视口和裁剪
        VkViewport m_viewport;
        VkRect2D m_scissor;
        // 交换链
        VkSwapchainKHR m_swapchain;
        VkFormat m_swapchain_image_format;
        // 交换范围
        VkExtent2D m_swapchain_extent;
        // 交换链图像
        std::vector<VkImage> m_swapchain_images;
        std::vector<VkImageView> m_swapchain_image_views;

        // 深度图像
        AllocatedImage m_depth_image;
        VkImageView m_depth_image_view;
        VkFormat m_depth_format;
    };
} // namespace Yutrel