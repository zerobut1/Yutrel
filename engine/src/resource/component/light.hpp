#pragma once

#include <glm/glm.hpp>

namespace Yutrel
{
    struct Light
    {
    public:
        Light() = default;

    public:
        glm::vec3 color{1.0f, 1.0f, 1.0f};
        float intensity{100.0f};
    };

    struct DirectionalLight : public Light
    {
    public:
        DirectionalLight() = default;

    public:
        glm::vec3 direction;
    };

} // namespace Yutrel