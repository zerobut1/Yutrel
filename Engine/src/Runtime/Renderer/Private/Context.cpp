#include "Context.h"

#include "Log.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>

namespace Yutrel
{
    Context::Context(const CreateInfo& info)
    {
        init(info);
    }

    Context::~Context()
    {
        destroy();
    }

    void Context::init(const CreateInfo& info)
    {
        //--------------instance------------------
        // todo instance拓展
        auto inst_ret =
            vkb::InstanceBuilder()
                .set_app_name("Vulkan RHI")
                .request_validation_layers(info.enable_validation_layers)
                .use_default_debug_messenger()
                .require_api_version(1, 3, 0)
                .build();

        if (!inst_ret)
        {
            destroy();
            LOG_ERROR("Failed to create Vulkan instance.\n");
            return;
        }

        vkb::Instance vkb_instance = inst_ret.value();

        // 获取instance
        m_instance        = vkb_instance.instance;
        m_debug_messenger = vkb_instance.debug_messenger;

        //--------------GPU------------------
        VkPhysicalDeviceVulkan12Features device_features_12 = info.device_features_12;
        VkPhysicalDeviceVulkan13Features device_features_13 = info.device_features_13;

        // 选择物理设备
        vkb::PhysicalDeviceSelector selector{vkb_instance};
        vkb::PhysicalDevice physical_device =
            selector
                .set_minimum_version(1, 3)
                .defer_surface_initialization()
                .set_required_features(info.device_features)
                .set_required_features_12(device_features_12)
                .set_required_features_13(device_features_13)
                .select()
                .value();

        // 创建逻辑设备
        vkb::DeviceBuilder device_builder{physical_device};

        vkb::Device vkb_device = device_builder.build().value();

        // 获取设备
        m_GPU            = physical_device.physical_device;
        m_GPU_properties = vkb_device.physical_device.properties;
        m_device         = vkb_device.device;

        // 获取图形队列
        m_graphics_queue              = vkb_device.get_queue(vkb::QueueType::graphics).value();
        m_graphics_queue_family_index = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
    }

    void Context::destroy()
    {
        // 销毁逻辑设备
        m_device.destroy();

        // 销毁debug messenger
        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);

        // 销毁vulkan实例
        m_instance.destroy();
    }

} // namespace Yutrel