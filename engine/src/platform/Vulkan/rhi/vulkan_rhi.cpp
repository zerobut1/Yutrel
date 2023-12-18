#include "yutrel_pch.hpp"

#include "vulkan_rhi.hpp"

#include <GLFW/glfw3.h>
#include <VKBootstrap.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace Yutrel
{
    void VulkanRHI::Init(RHIInitInfo info)
    {
        InitVulkan(info.raw_window);
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

    void VulkanRHI::InitVulkan(GLFWwindow* raw_window)
    {
        LOG_INFO("Initialize Vulkan RHI");

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

} // namespace Yutrel