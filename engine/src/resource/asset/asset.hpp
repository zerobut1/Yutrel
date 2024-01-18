#pragma once

#include "core/macro.hpp"
#include "resource/component/component.hpp"

#include <gecs/gecs.hpp>
#include <tiny_gltf.h>

#include <stdint.h>
#include <string>
#include <vector>

namespace Yutrel
{
    class AssetManager
    {
    public:
        Ref<struct Mesh> AddMesh(const std::string& path);

        Ref<struct Material> AddMaterial(const Material& material);

        Ref<struct Texture> AddTexture(const std::string& path);

        Ref<struct GLTFScene> AddGLTFScene(const std::string& path);

        void LoadFromFile(Ref<Mesh> mesh);

        void LoadFromFile(Ref<Texture> texture);

        void LoadFromFile(gecs::entity scene_entity, Ref<GLTFScene> scene, const Transform& transform, Children& children, gecs::commands& cmd);

        void LoadNode(const tinygltf::Node& in_node,
                      const tinygltf::Model& model,
                      const Transform& transform,
                      const std::vector<Ref<Material>>& materials,
                      gecs::entity* parent,
                      Children& children,
                      gecs::commands& cmd);

        static void LoadMeshes(gecs::querier<Ref<Mesh>> meshes,
                               gecs::resource<gecs::mut<AssetManager>> asset_manager);

        static void LoadMaterials(gecs::querier<Ref<Material>> materials,
                                  gecs::resource<gecs::mut<AssetManager>> asset_manager);

        static void LoadGLTFScenes(gecs::querier<Ref<GLTFScene>, Transform, gecs::mut<Children>> scenes,
                                   gecs::resource<gecs::mut<AssetManager>> asset_manager,
                                   gecs::commands cmd);

    private:
        std::vector<Ref<Mesh>> m_meshes;

        std::vector<Ref<Material>> m_materials;

        std::vector<Ref<Texture>> m_textures;

        std::vector<Ref<GLTFScene>> m_gltf_scenes;
    };
} // namespace Yutrel