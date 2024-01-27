#pragma once

#include "platform/Vulkan/vulkan_types.hpp"
#include "resource/asset/material.hpp"
#include "resource/asset/texture.hpp"

namespace Yutrel
{
    struct MaterialUniformData
    {
        glm::vec4 base_color_factor;
        float metallic_factor;
        float roughness_factor;
        float padding[2];
    };

    struct VulkanPBRMaterial
    {
        AllocatedImage base_color_texture;
        AllocatedImage metallic_roughness_texture;
        AllocatedImage normal_texture;

        AllocatedBuffer uniform_buffer;

        VkDescriptorSet descriptor_set;
    };
} // namespace Yutrel