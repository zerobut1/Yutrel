#pragma once

#include "platform/Vulkan/vulkan_types.hpp"

namespace Yutrel
{
    struct VulkanSkybox
    {
        AllocatedImage irradiance;
        AllocatedImage prefiltered;

        Ref<struct VulkanMesh> cube;
    };
} // namespace Yutrel