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
        alignas(16) glm::vec3 pos;
        alignas(16) glm::vec3 normal;
        alignas(16) glm::vec3 tangent;
        alignas(8) glm::vec2 uv;

        bool operator==(const Vertex& other) const
        {
            return pos == other.pos &&
                   normal == other.normal &&
                   uv == other.uv;
        }
    };

    struct Mesh
    {
    public:
        Mesh() = default;
        Mesh(const std::string& path)
            : path(path) {}

        void ReleaseVertices()
        {
            vertices.reset();
            indices.reset();
        }

    public:
        std::string path;
        bool is_loaded{false};

        Ref<std::vector<Vertex>> vertices;
        Ref<std::vector<uint32_t>> indices;
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
            return ((hash<glm::vec3>()(vertex.pos) ^
                     (hash<glm::vec3>()(vertex.normal) << 1)) >>
                    1) ^
                   (hash<glm::vec2>()(vertex.uv) << 1);
        }
    };
} // namespace std