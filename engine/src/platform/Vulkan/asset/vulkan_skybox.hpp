#pragma once

#include "platform/Vulkan/vulkan_types.hpp"

namespace Yutrel
{
    struct VulkanSkybox
    {
        AllocatedImage irradiance;
        AllocatedImage prefiltered;

        AllocatedImage brdf_lut;

        Ref<struct VulkanMesh> cube;
    };
} // namespace Yutrel