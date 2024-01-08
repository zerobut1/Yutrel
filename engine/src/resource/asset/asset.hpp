#pragma once

#include "core/macro.hpp"

#include <stdint.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Yutrel
{
    class AssetManager
    {
    public:
        Ref<struct Mesh> AddMesh(const std::string& path);

        Ref<struct Material> AddMaterial(const Material& material);

        void LoadFromFile(Ref<Mesh> mesh);

    private:
        std::unordered_map<uint32_t, Ref<Mesh>> m_meshes;

        std::unordered_map<uint32_t, Ref<Material>> m_materials;
    };

    // class Asset
    // {

    // public:
    //     uint32_t ID() const { return this->m_id; }

    // protected:
    //     Asset()          = default;
    //     virtual ~Asset() = default;

    //     Asset(const Asset&)            = delete;
    //     Asset& operator=(const Asset&) = delete;
    //     Asset(Asset&& other) noexcept;
    //     Asset& operator=(Asset&& other) noexcept;

    //     virtual void Bind() const {}
    //     virtual void Unbind() const {}

    //     virtual void Bind(uint32_t index) const {}
    //     virtual void Unbind(uint32_t index) const {}

    // protected:
    //     uint32_t m_id;
    // };
} // namespace Yutrel