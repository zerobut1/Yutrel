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
        glm::mat4 model_matrix;
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

    struct BackGroundColor
    {
        glm::vec4 data1;
        glm::vec4 data2;
        glm::vec4 data3;
        glm::vec4 data4;
    };

} // namespace Yutrel