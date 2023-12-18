/**
 * 光照
 * 点光源、平行光、聚光
 */
#pragma once

#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <limits>

namespace Yutrel
{
    class Light
    {
    public:
        Light() = default;
        Light(const glm::vec3& color, float intensity = 1.0f);

    public:
        glm::vec3 color;
        float intensity;
    };

    class DirectionLight : public Light
    {
    public:
        DirectionLight() = default;
        DirectionLight(const glm::vec3& color, const glm::vec3& direction, float intensity = 1.0f);

    public:
        glm::vec3 direction;
    };

    class PointLight : public Light
    {
    public:
        using Light::Light;

        void SetAttenuation(float linear, float quadratic);
        float GetAttenuation(float distance) const;

    public:
        float linear, quadratic;
        float range = std::numeric_limits<float>::max();
    };

    class Spotlight : public Light
    {
    public:
        using Light::Light;

        void SetCutoff(float range, float inner_cutoff = 15.0f, float outer_cutoff = 30.0f);
        float GetInnerCosine() const;
        float GetOuterCosine() const;
        float GetAttenuation(float distance) const;

    public:
        float inner_cutoff; // angle in degrees at the base of the inner cone
        float outer_cutoff; // angle in degrees at the base of the outer cone
        float range = std::numeric_limits<float>::max();
    };

    class AreaLight : public Light
    {
    public:
        using Light::Light;

        // TODO: to be implemented, using Bezier curves sampling and LTC
    };

    class VolumeLight : public Light
    {
    public:
        using Light::Light;

        // TODO: to be implemented, requires volumetric path tracing
    };

} // namespace Yutrel