#include "yutrel_pch.hpp"

#include "vulkan_rhi.hpp"

// #include "platform/Vulkan/asset/vulkan_mesh.hpp"
// #include "platform/Vulkan/initializers/initializers.hpp"
// #include "platform/Vulkan/utils/vulkan_utils.hpp"

#include <GLFW/glfw3.h>
#include <VKBootstrap.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <stdint.h>
#include <vcruntime.h>
#include <vk_mem_alloc_enums.hpp>
#include <vk_mem_alloc_handles.hpp>
#include <vk_mem_alloc_structs.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace Yutrel
{
    void VulkanRHI::Init(RHIInitInfo info)
    {
        InitVulkan(info.raw_window);

        InitSwapchain(info.width, info.height);

        InitCommands();

        InitSyncStructures();

        // InitDescriptorPool();

        // InitImgui(info.raw_window);
    }

    void VulkanRHI::Clear()
    {
        // 确保GPU已经工作完
        vkDeviceWaitIdle(m_device);

        // 删除队列清空
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            m_frames[i].deletion_queue.flush();
        }
        m_main_deletion_queue.flush();

        // 销毁交换链
        DestroySwapchain();

        // 销毁内存分配器
        m_allocator.destroy();

        // 销毁窗口表面
        m_instance.destroySurfaceKHR(m_surface);

        // 销毁逻辑设备
        m_device.destroy();

        // 销毁debug messenger
        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);

        // 销毁vulkan实例
        m_instance.destroy();
    }

    void VulkanRHI::InitVulkan(GLFWwindow* raw_window)
    {
        // vkb创建器
        vkb::InstanceBuilder builder;

        // 创建vulkan instance
        auto inst_ret =
            builder.set_app_name("Vulkan RHI")
                .request_validation_layers(m_enable_validation_layers)
                .use_default_debug_messenger()
                .require_api_version(1, 3, 0)
                .build();

        vkb::Instance vkb_instance = inst_ret.value();

        // 获取instance
        m_instance        = vkb_instance.instance;
        m_debug_messenger = vkb_instance.debug_messenger;

        // 创建窗口表面
        VkSurfaceKHR surface;
        glfwCreateWindowSurface(m_instance, raw_window, nullptr, &surface);
        m_surface = static_cast<vk::SurfaceKHR>(surface);

        // 设备特性
        VkPhysicalDeviceFeatures device_features{};
        device_features.samplerAnisotropy = vk::True;

        // vulkan 1.2 特性
        VkPhysicalDeviceVulkan12Features features_12{};
        features_12.bufferDeviceAddress = vk::True;
        features_12.descriptorIndexing  = vk::True;

        // vulkan 1.3 特性
        VkPhysicalDeviceVulkan13Features features_13{};
        features_13.dynamicRendering = vk::True;
        features_13.synchronization2 = vk::True;

        // 选择物理设备
        vkb::PhysicalDeviceSelector selector{vkb_instance};
        vkb::PhysicalDevice physical_device =
            selector
                .set_minimum_version(1, 3)
                .set_required_features(device_features)
                .set_required_features_12(features_12)
                .set_required_features_13(features_13)
                .set_surface(m_surface)
                .select()
                .value();

        // 创建逻辑设备
        vkb::DeviceBuilder device_builder{physical_device};

        vkb::Device vkb_device = device_builder
                                     .build()
                                     .value();

        // 获取设备
        m_GPU            = physical_device.physical_device;
        m_GPU_properties = vkb_device.physical_device.properties;
        m_device         = vkb_device.device;

        // 获取图形队列
        m_graphics_queue        = vkb_device.get_queue(vkb::QueueType::graphics).value();
        m_graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

        // 初始化内存分配器
        auto allocator_ci =
            vma::AllocatorCreateInfo()
                .setPhysicalDevice(m_GPU)
                .setDevice(m_device)
                .setInstance(m_instance)
                .setFlags(vma::AllocatorCreateFlagBits::eBufferDeviceAddress);

        m_allocator = vma::createAllocator(allocator_ci);
    }

    void VulkanRHI::InitSwapchain(uint32_t width, uint32_t height)
    {
        //---------vkb创建交换链-----------
        vkb::SwapchainBuilder swapchain_builder{m_GPU, m_device, m_surface};

        vkb::Swapchain vkb_swapchain =
            swapchain_builder
                .use_default_format_selection()
                .set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
                .set_desired_extent(width, height)
                .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                .build()
                .value();

        // 获取交换链和图像
        m_swapchain                = vkb_swapchain.swapchain;
        m_swapchain_extent         = vkb_swapchain.extent;
        m_swapchain_format         = static_cast<vk::Format>(vkb_swapchain.image_format);
        auto swapchain_images      = vkb_swapchain.get_images().value();
        auto swapchain_image_views = vkb_swapchain.get_image_views().value();
        for (auto image : swapchain_images)
        {
            m_swapchain_images.push_back(static_cast<vk::Image>(image));
        }
        for (auto&& view : swapchain_image_views)
        {
            m_swapchain_image_views.push_back(static_cast<vk::ImageView>(view));
        }
    }

    void VulkanRHI::DestroySwapchain()
    {
        m_device.destroySwapchainKHR(m_swapchain);

        for (auto view : m_swapchain_image_views)
        {
            m_device.destroyImageView(view);
        }
    }

    void VulkanRHI::InitCommands()
    {
        // 为并行的每一帧分别创建指令池
        auto cmd_pool_ci =
            vk::CommandPoolCreateInfo()
                .setQueueFamilyIndex(m_graphics_queue_family)
                .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            m_frames[i].cmd_pool = m_device.createCommandPool(cmd_pool_ci);

            // 分配指令缓冲
            auto cmd_buffer_ai =
                vk::CommandBufferAllocateInfo()
                    .setCommandPool(m_frames[i].cmd_pool)
                    .setCommandBufferCount(1)
                    .setLevel(vk::CommandBufferLevel::ePrimary);

            m_frames[i].main_cmd_buffer = m_device.allocateCommandBuffers(cmd_buffer_ai).front();

            // 放入删除队列
            m_main_deletion_queue.PushFunction(
                [=]()
                {
                    m_device.destroyCommandPool(m_frames[i].cmd_pool);
                });
        }

        // 创建单次指令指令池
        cmd_pool_ci.setFlags({});
        m_rhi_cmd_pool = m_device.createCommandPool(cmd_pool_ci);
        m_main_deletion_queue.PushFunction(
            [=]()
            {
                m_device.destroyCommandPool(m_rhi_cmd_pool);
            });
    }

    void VulkanRHI::InitSyncStructures()
    {
        // 同步设施创建信息
        auto fence_ci =
            vk::FenceCreateInfo()
                .setFlags(vk::FenceCreateFlagBits::eSignaled);
        auto semaphore_ci =
            vk::SemaphoreCreateInfo()
                .setFlags({});

        // 为并行的每一帧创建
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            m_frames[i].render_fence = m_device.createFence(fence_ci);

            // 放入删除队列
            m_main_deletion_queue.PushFunction(
                [=]()
                {
                    m_device.destroyFence(m_frames[i].render_fence);
                });

            m_frames[i].finished_for_presentation_semaphore = m_device.createSemaphore(semaphore_ci);
            m_frames[i].available_for_render_semaphore      = m_device.createSemaphore(semaphore_ci);

            // 放入删除队列
            m_main_deletion_queue.PushFunction(
                [=]()
                {
                    m_device.destroySemaphore(m_frames[i].finished_for_presentation_semaphore);
                    m_device.destroySemaphore(m_frames[i].available_for_render_semaphore);
                });
        }
    }

} // namespace Yutrel