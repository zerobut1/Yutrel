#include "yutrel_pch.hpp"

#include "light.hpp"

namespace Yutrel
{
    Light::Light(const glm::vec3& color, float intensity)
        : color(color), intensity(intensity)
    {
    }

    DirectionalLight::DirectionalLight(const glm::vec3& color, const glm::vec3& direction, float intensity)
        : Light(color, intensity), direction(direction)
    {
    }

    // 设置衰减
    void PointLight::SetAttenuation(float linear, float quadratic)
    {
        YUTREL_ASSERT(linear > 0, "The linear attenuation factor must be positive ...");
        YUTREL_ASSERT(quadratic > 0, "The quadratic attenuation factor must be positive ...");

        this->linear    = linear;
        this->quadratic = quadratic;

        // find the max range by solving the quadratic equation when attenuation is <= 0.01
        float a     = quadratic;
        float b     = linear;
        float c     = -100.0f;
        float delta = b * b - 4.0f * a * c;

        YUTREL_ASSERT(delta > 0.0f, "You can never see this line, it is mathematically impossible...");
        range = c / (-0.5f * (b + sqrt(delta))); // Muller's method
    }

    float PointLight::GetAttenuation(float distance) const
    {
        // our point light follows the inverse-square law, so the attenuation is inversely
        // proportional to the square of distance. While it doesn't have a true concept of
        // "range" in the physics sense, we can still approximate a range with a threshold
        // value at which attenuation becomes really small (such as 0.01), distance out of
        // this range will be considered to have an attenuation of 0.

        YUTREL_ASSERT(distance >= 0.0f, "Distance to the light source cannot be negative ...");
        return distance >= range ? 0.0f
                                 : 1.0f / (1.0f + linear * distance + quadratic * pow(distance, 2.0f));
    }

    void Spotlight::SetCutoff(float range, float inner_cutoff, float outer_cutoff)
    {
        YUTREL_ASSERT(range > 0, "The spotlight range must be positive ...");
        YUTREL_ASSERT(inner_cutoff > 0, "The inner cutoff angle must be positive ...");
        YUTREL_ASSERT(outer_cutoff > 0, "The outer cutoff angle must be positive ...");

        this->range        = range;
        this->inner_cutoff = inner_cutoff;
        this->outer_cutoff = outer_cutoff;
    }

    float Spotlight::GetInnerCosine() const
    {
        return glm::cos(glm::radians(inner_cutoff));
    }

    float Spotlight::GetOuterCosine() const
    {
        return glm::cos(glm::radians(outer_cutoff));
    }

    float Spotlight::GetAttenuation(float distance) const
    {
        // to keep it simple, our spotlight does not follow the inverse-square law, a linear
        // falloff is used instead for the attenuation over distance, despite that, we still
        // have a fade-out effect from the inner cone to the outer cone along the radius, so
        // the overall effect would be realistic enough without loss of fidelity.

        YUTREL_ASSERT(distance >= 0.0f, "Distance to the light source cannot be negative...");
        return 1.0f - std::clamp(distance / range, 0.0f, 1.0f);
    }

} // namespace Yutrel