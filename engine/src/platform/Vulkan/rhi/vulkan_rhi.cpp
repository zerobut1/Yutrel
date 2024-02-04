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
#include <vk_mem_alloc_enums.hpp>
#include <vk_mem_alloc_handles.hpp>
#include <vk_mem_alloc_structs.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace Yutrel
{
    void VulkanRHI::Init(RHIInitInfo info)
    {
        InitVulkan(info.raw_window);

        // InitSwapchain(info.width, info.height);

        // InitCommands();

        // InitSyncStructures();

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
        // DestroySwapchain();

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
        device_features.samplerAnisotropy = VK_TRUE;

        // vulkan 1.2 特性
        VkPhysicalDeviceVulkan12Features features_12{};
        features_12.bufferDeviceAddress = true;
        features_12.descriptorIndexing  = true;

        // vulkan 1.3 特性
        VkPhysicalDeviceVulkan13Features features_13{};
        features_13.dynamicRendering = true;
        features_13.synchronization2 = true;

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

} // namespace Yutrel