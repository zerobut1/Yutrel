#include "context.h"

#include "Core/log.h"

#include <VKBootstrap.h>

namespace Yutrel
{
    Context::~Context()
    {
        Destroy();
    }

    bool Context::Init(const ContextCreateInfo& info)
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
            Destroy();
            LOG_ERROR("Failed to create Vulkan instance.\n");
            return false;
        }

        vkb::Instance vkb_instance = inst_ret.value();

        // 获取instance
        m_instance        = vkb_instance.instance;
        m_debug_messenger = vkb_instance.debug_messenger;

        //-------------GPU-------------------------
        // 设备特性
        vk::PhysicalDeviceFeatures device_features{};
        device_features.samplerAnisotropy = vk::True;
        device_features.fillModeNonSolid  = vk::True;

        // vulkan 1.2 特性
        vk::PhysicalDeviceVulkan12Features features_12{};
        features_12.bufferDeviceAddress = vk::True;
        features_12.descriptorIndexing  = vk::True;

        // vulkan 1.3 特性
        vk::PhysicalDeviceVulkan13Features features_13{};
        features_13.dynamicRendering = vk::True;
        features_13.synchronization2 = vk::True;

        // 选择物理设备
        // vkb::PhysicalDeviceSelector selector{vkb_instance};
        // vkb::PhysicalDevice physical_device =
        //     selector
        //         .set_minimum_version(1, 3)
        //         .set_required_features(device_features)
        //         .set_required_features_12(features_12)
        //         .set_required_features_13(features_13)
        //         .set_surface(m_surface)
        //         .select()
        //         .value();

        // // 创建逻辑设备
        // vkb::DeviceBuilder device_builder{physical_device};

        // vkb::Device vkb_device = device_builder
        //                              .build()
        //                              .value();

        // // 获取设备
        // m_GPU            = physical_device.physical_device;
        // m_GPU_properties = vkb_device.physical_device.properties;
        // m_device         = vkb_device.device;

        // // 获取图形队列
        // m_graphics_queue        = vkb_device.get_queue(vkb::QueueType::graphics).value();
        // m_graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

        return true;
    }

    void Context::Destroy()
    {
    }

} // namespace Yutrel