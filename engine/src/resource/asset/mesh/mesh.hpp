#pragma once

#include "core/macro.hpp"

#include <glm/glm.hpp>

#include <stdint.h>
#include <string>
#include <vector>

namespace Yutrel
{
    struct VertexInputDescription;
    struct GPUMeshBuffers;

    // struct Vertex
    // {
    //     glm::vec3 position;
    //     glm::vec3 normal;
    //     glm::vec3 color;
    //     glm::vec2 uv;

    //     static VertexInputDescription GetVertexDescription();
    // };

    struct Vertex
    {
        glm::vec3 position;
        float uv_x;
        glm::vec3 normal;
        float uv_y;
        glm::vec4 color;

        static VertexInputDescription GetVertexDescription();
    };

    class Mesh
    {
    public:
        Mesh() = default;
        Mesh(const std::string& path)
            : path(path) {}

        void ReleaseVertices()
        {
            vertices.reset();
            is_loaded = false;
        }

    public:
        std::string path;
        bool is_loaded{false};
        bool is_uploaded{false};

        Ref<std::vector<Vertex>> vertices;
        Ref<std::vector<uint32_t>> indices;

        Ref<GPUMeshBuffers> gpu_buffers;
    };

} // namespace Yutrel
