#include "yutrel_pch.hpp"

#include "vulkan_rhi.hpp"

#include "platform/Vulkan/initializers/initializers.hpp"

#include <GLFW/glfw3.h>
#include <VKBootstrap.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace Yutrel
{
    void VulkanRHI::Init(RHIInitInfo info)
    {
        InitVulkan(info.raw_window, info.width, info.height);

        InitCommands();

        // InitSwapchain();
    }

    void VulkanRHI::Clear()
    {
        // 删除队列清空
        main_deletion_queue.flush();

        // 销毁窗口表面
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

        // 销毁逻辑设备
        vkDestroyDevice(m_device, nullptr);

        // 销毁debug messenger
        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);

        // 销毁vulkan实例
        vkDestroyInstance(m_instance, nullptr);
    }

    void VulkanRHI::InitVulkan(GLFWwindow* raw_window, uint32_t width, uint32_t height)
    {
        LOG_INFO("Initialize Vulkan RHI");

        // 记录交换范围大小
        m_window_extent.width  = width;
        m_window_extent.height = height;

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
        glfwCreateWindowSurface(m_instance, raw_window, nullptr, &m_surface);

        // 选择物理设备
        vkb::PhysicalDeviceSelector selector{vkb_instance};
        vkb::PhysicalDevice physical_device =
            selector
                .set_minimum_version(1, 3)
                .set_surface(m_surface)
                .select()
                .value();

        // 创建逻辑设备
        vkb::DeviceBuilder device_builder{physical_device};
        // 启用shader draw parameters功能
        VkPhysicalDeviceShaderDrawParametersFeatures shader_draw_parameters_features{};
        shader_draw_parameters_features.sType                = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES;
        shader_draw_parameters_features.pNext                = nullptr;
        shader_draw_parameters_features.shaderDrawParameters = VK_TRUE;

        vkb::Device vkb_device = device_builder
                                     .add_pNext(&shader_draw_parameters_features)
                                     .build()
                                     .value();

        // 获取设备
        m_physical_device            = physical_device.physical_device;
        m_physical_device_properties = vkb_device.physical_device.properties;
        m_device                     = vkb_device.device;

        // 获取图形队列
        m_graphics_queue        = vkb_device.get_queue(vkb::QueueType::graphics).value();
        m_graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

        // 初始化内存分配器
        VmaAllocatorCreateInfo allocator_info{};
        allocator_info.physicalDevice = m_physical_device;
        allocator_info.device         = m_device;
        allocator_info.instance       = m_instance;
        vmaCreateAllocator(&allocator_info, &allocator);

        // 加入销毁队列
        main_deletion_queue
            .PushFunction([=]()
                          { vmaDestroyAllocator(allocator); });
    }

    void VulkanRHI::InitSwapchain()
    {
        // vkb创建交换链
        vkb::SwapchainBuilder swapchain_builder{m_physical_device, m_device, m_surface};

        vkb::Swapchain vkb_swapchain =
            swapchain_builder
                .use_default_format_selection()
                // 设为FIFO，使其限制为显示器帧数
                .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                .set_desired_extent(m_window_extent.width, m_window_extent.height)
                .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                .build()
                .value();

        // 获取交换链和图像
        m_swapchain              = vkb_swapchain.swapchain;
        m_swapchain_image_format = vkb_swapchain.image_format;
        m_swapchain_images       = vkb_swapchain.get_images().value();
        m_swapchain_image_views  = vkb_swapchain.get_image_views().value();

        // 放入删除队列
        main_deletion_queue
            .PushFunction([=]()
                          { vkDestroySwapchainKHR(m_device, m_swapchain, nullptr); });
    }

    void VulkanRHI::InitCommands()
    {
        // 创建指令池
        VkCommandPoolCreateInfo cmd_pool_info = vkinit::CommandPoolCreateInfo(m_graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        // 为并行的每一帧分别创建指令池
        for (int i = 0; i < FRAME_OVERLAP; i++)
        {
            YUTREL_ASSERT(vkCreateCommandPool(m_device, &cmd_pool_info, nullptr, &m_frames[i].command_pool) == VK_SUCCESS, "Failed to create command pool");

            // 分配指令缓冲
            auto cmd_alloc_info = vkinit::CommandBufferAllocateInfo(m_frames[i].command_pool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

            YUTREL_ASSERT(vkAllocateCommandBuffers(m_device, &cmd_alloc_info, &m_frames[i].main_command_buffer) == VK_SUCCESS, "Failed to allocate command buffer");

            // 放入删除队列
            main_deletion_queue
                .PushFunction([=]()
                              { vkDestroyCommandPool(m_device, m_frames[i].command_pool, nullptr); });
        }

        // 创建上传指令池
        VkCommandPoolCreateInfo upload_command_pool_info = vkinit::CommandPoolCreateInfo(m_graphics_queue_family, 0);

        YUTREL_ASSERT(vkCreateCommandPool(m_device, &upload_command_pool_info, nullptr, &m_upload_context.command_pool) == VK_SUCCESS, "Failed to create upload command pool");

        main_deletion_queue
            .PushFunction([=]()
                          { vkDestroyCommandPool(m_device, m_upload_context.command_pool, nullptr); });

        // 分配上传指令缓冲
        VkCommandBufferAllocateInfo cmd_alloc_info = vkinit::CommandBufferAllocateInfo(m_upload_context.command_pool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        YUTREL_ASSERT(vkAllocateCommandBuffers(m_device, &cmd_alloc_info, &m_upload_context.command_buffer) == VK_SUCCESS, "Failed to allocate upload command buffer");
    }

} // namespace Yutrel