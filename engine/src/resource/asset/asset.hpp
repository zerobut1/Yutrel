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

        Ref<struct Texture> AddTexture(const std::string& path);

        void LoadFromFile(Ref<Mesh> mesh);

        void LoadFromFile(Ref<Texture> texture);

    private:
        std::unordered_map<uint32_t, Ref<Mesh>> m_meshes;

        std::unordered_map<uint32_t, Ref<Material>> m_materials;

        std::unordered_map<uint32_t, Ref<Texture>> m_textures;
    };
} // namespace Yutrel