#pragma once

#include "core/macro.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <vector>

namespace Yutrel
{
    struct VertexInputDescription;

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
        glm::vec2 uv;

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
        Ref<struct AllocatedBuffer> vertex_buffer;
    };

} // namespace Yutrel
