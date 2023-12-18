/**
 * 网格
 * 包含顶点数据，对应的material，以及AABB包围盒
 */
#pragma once

#include "glm/fwd.hpp"
#include "resource/asset/texture.hpp"
#include "resource/asset/vertex_array.hpp"
#include "resource/component/model/frustum.hpp"
#include "resource/component/model/material.hpp"

#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

namespace Yutrel
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;
        glm::vec3 bitangent;
        glm::vec2 uv;
        glm::ivec4 bone_ids    = glm::ivec4(-1);  // 4 bones per vertex rule
        glm::vec4 bone_weights = glm::vec4(0.0f); // the weight of each bone
    };

    // 网格类
    class Mesh
    {
        friend class Model;

    public:
        static Ref<Mesh> Create(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

    public:
        Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
        virtual ~Mesh() = default;

        void SetMaterialName(std::string& name) const;
        std::string GetMaterialName() const { return m_material_name; }

    private:
        Ref<VertexArray> m_vertex_array;
        Scope<AABB> m_AABB;
        mutable std::string m_material_name;
    };
} // namespace Yutrel