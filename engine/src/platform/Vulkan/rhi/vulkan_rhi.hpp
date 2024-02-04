#pragma once

#include "core/macro.hpp"
#include "platform/Vulkan/vulkan_types.hpp"
// #include "platform/Vulkan/utils/vulkan_utils.hpp"

#include <array>
#include <deque>
#include <functional>
#include <stdint.h>
#include <vector>
#include <vk_mem_alloc_handles.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

struct GLFWwindow;

namespace Yutrel
{
    struct RHIInitInfo
    {
        GLFWwindow* raw_window;
        uint32_t width;
        uint32_t height;
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

    class VulkanRHI
    {
    public:
        void Init(RHIInitInfo info);

        void Clear();

    private:
        //--------初始化----------
        void InitVulkan(GLFWwindow* raw_window);
        // 指令池与指令缓冲
        void InitCommands();
        // 描述符
        void InitDescriptorPool();
        // 同步设施
        void InitSyncStructures();
        // 交换链
        void InitSwapchain(uint32_t width, uint32_t height);
        // Imgui
        void InitImgui(GLFWwindow* raw_window);
        //------------------------

    private:
        // 同时渲染的帧数
        static constexpr uint8_t MAX_FRAMES_IN_FLIGHT{2};

        // 允许验证层
        bool m_enable_validation_layers{true};

        // 当前帧数
        uint32_t m_cur_frame{0};

        // 删除队列
        DeletionQueue m_main_deletion_queue;

        vk::Instance m_instance;
        vk::DebugUtilsMessengerEXT m_debug_messenger;
        vk::SurfaceKHR m_surface;
        vk::PhysicalDevice m_GPU;
        vk::PhysicalDeviceProperties m_GPU_properties;
        vk::Device m_device;
        vk::Queue m_graphics_queue;
        uint32_t m_graphics_queue_family;

        vma::Allocator m_allocator;

        // 帧数据
        std::array<FrameData, MAX_FRAMES_IN_FLIGHT> m_frames;
    };
} // namespace Yutrel