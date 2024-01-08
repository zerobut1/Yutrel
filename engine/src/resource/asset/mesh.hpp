#pragma once

#include "core/macro.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <stdint.h>
#include <string>
#include <vector>

namespace Yutrel
{
    struct Vertex
    {
        alignas(16) glm::vec3 position;
        alignas(16) glm::vec3 normal;
        alignas(8) glm::vec2 uv;

        bool operator==(const Vertex& other) const
        {
            return position == other.position &&
                   normal == other.normal &&
                   uv == other.uv;
        }
    };

    struct Mesh
    {
        std::string path;
        bool is_loaded{false};
        bool is_uploaded{false};

        Ref<std::vector<Vertex>> vertices;
        Ref<std::vector<uint32_t>> indices;

        Mesh() = default;
        Mesh(const std::string& path)
            : path(path) {}

        void ReleaseVertices()
        {
            vertices.reset();
            indices.reset();
        }
    };

} // namespace Yutrel

// 哈希函数
namespace std
{
    template <>
    struct hash<Yutrel::Vertex>
    {
        size_t operator()(Yutrel::Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.position) ^
                     (hash<glm::vec3>()(vertex.normal) << 1)) >>
                    1) ^
                   (hash<glm::vec2>()(vertex.uv) << 1);
        }
    };
} // namespace std