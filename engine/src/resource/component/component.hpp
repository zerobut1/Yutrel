#pragma once

#include "core/macro.hpp"

#include <glm/glm.hpp>

#include <gecs/gecs.hpp>

#include <vector>

namespace Yutrel
{
    struct Parent
    {
        gecs::entity entity;
    };

    struct Children
    {
        std::vector<gecs::entity> entities;
    };

    struct Transform
    {
        glm::mat4 model_matrix{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    };

    struct PbrBundle
    {
        Ref<struct Mesh> mesh;
        Ref<struct Material> material;
        Transform transform;
    };

    struct SceneBundle
    {
        Ref<struct GLTFScene> scene;
        Transform transform;
        Children children;
    };

    struct Skybox
    {
        Ref<struct Texture> brdf_lut;

        Ref<Texture> irradiace_pos_x_map;
        Ref<Texture> irradiace_neg_x_map;
        Ref<Texture> irradiace_pos_y_map;
        Ref<Texture> irradiace_neg_y_map;
        Ref<Texture> irradiace_pos_z_map;
        Ref<Texture> irradiace_neg_z_map;

        Ref<Texture> prefiltered_pos_x_map;
        Ref<Texture> prefiltered_neg_x_map;
        Ref<Texture> prefiltered_pos_y_map;
        Ref<Texture> prefiltered_neg_y_map;
        Ref<Texture> prefiltered_pos_z_map;
        Ref<Texture> prefiltered_neg_z_map;

        Ref<Mesh> cube;
    };

} // namespace Yutrel