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

    struct GPUMeshBuffers
    {
        AllocatedBuffer index_buffer;
        AllocatedBuffer vertex_buffer;
        VkDeviceAddress vertex_buffer_address;
    };
} // namespace Yutrel