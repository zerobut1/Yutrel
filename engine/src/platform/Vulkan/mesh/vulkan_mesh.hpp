#pragma once

#include "resource/asset/mesh/mesh.hpp"

#include "platform/Vulkan/vulkan_types.hpp"

namespace Yutrel
{
    struct VertexInputDescription
    {
        std::vector<VkVertexInputBindingDescription> bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;

        VkPipelineVertexInputStateCreateFlags flags{0};
    };
} // namespace Yutrel