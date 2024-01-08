#pragma once

#include "platform/Vulkan/vulkan_types.hpp"
#include "resource/asset/material.hpp"

namespace Yutrel
{
    // todo 自动填充
    struct VulkanMaterialData
    {
        glm::vec4 base_color;
        glm::vec4 padding[15];
    };

    struct VulkanPBRMaterial
    {
        AllocatedBuffer uniform_buffer;
    };
} // namespace Yutrel