#pragma once

#include "resource/asset/mesh.hpp"

#include "platform/Vulkan/vulkan_types.hpp"

namespace Yutrel
{
    struct VulkanMesh
    {
        AllocatedBuffer index_buffer;
        AllocatedBuffer vertex_buffer;
        vk::DeviceAddress vertex_buffer_address;
        uint32_t index_count;
    };
} // namespace Yutrel