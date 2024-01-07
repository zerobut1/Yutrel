#pragma once

#include "resource/asset/mesh.hpp"

#include "platform/Vulkan/vulkan_types.hpp"

namespace Yutrel
{
    struct GPUMeshBuffers
    {
        AllocatedBuffer index_buffer;
        AllocatedBuffer vertex_buffer;
        VkDeviceAddress vertex_buffer_address;
        uint32_t index_count;
    };

    inline void Mesh::ReleaseVertices()
    {
        gpu_buffers->index_count = indices->size();

        vertices.reset();
        indices.reset();
    }
} // namespace Yutrel