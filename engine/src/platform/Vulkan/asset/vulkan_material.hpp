#pragma once

#include "platform/Vulkan/vulkan_types.hpp"
#include "resource/asset/material.hpp"
#include "resource/asset/texture.hpp"

namespace Yutrel
{
    // todo 自动填充
    struct MaterialUniformData
    {
        glm::vec4 base_color;
        glm::vec4 padding[15];
    };

    struct VulkanPBRMaterial
    {
        AllocatedImage base_color_texture;
        AllocatedImage normal_texture;

        AllocatedBuffer uniform_buffer;

        VkDescriptorSet descriptor_set;
    };
} // namespace Yutrel