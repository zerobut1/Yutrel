#pragma once

#include "platform/Vulkan/vulkan_types.hpp"

#include <glm/glm.hpp>

namespace Yutrel
{
    struct SceneUniformData
    {
        glm::mat4 view;
        glm::mat4 proj;
        glm::mat4 view_proj;
        glm::vec4 view_position;
        glm::vec4 ambient_color;
        glm::vec4 sunlight_direction; // w for sun power
        glm::vec4 sunlight_color;
    };

    struct DirectionalLightUniformData
    {
        glm::mat4 VP;
    };

    // struct GlobalRenderData
    // {
    //     AllocatedBuffer scene_buffer;
    //     AllocatedBuffer directional_light_buffer;
    // };
} // namespace Yutrel