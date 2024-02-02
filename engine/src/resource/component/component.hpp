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
        glm::mat4 model_matrix{
            1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
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

} // namespace Yutrel