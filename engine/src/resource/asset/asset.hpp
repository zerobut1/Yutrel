#pragma once

#include "resource/asset/mesh.hpp"

#include <stdint.h>
#include <string>
#include <unordered_map>

namespace Yutrel
{
    class AssetManager
    {
    public:
        Ref<Mesh> LoadMesh(const std::string& path);

        bool LoadFromFile(Ref<Mesh> mesh);

    private:
        std::unordered_map<std::string, Ref<Mesh>> m_meshes;
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