#pragma once

#define FLOAT_MAX         (asfloat(0x7F7FFFFF))
#define POSITIVE_INFINITY (asfloat(0x7F800000))
#define NEGATIVE_INFINITY (asfloat(0xFF800000))

#define PI = 3.1415926535897932385;

class Ray
{
    float3 origin;
    float3 direction;

    float3 at(float t)
    {
        return origin + direction * t;
    }
};

struct HitRecord
{
    float3 position;
    float3 normal;
    float t;
    bool front_face;

    void setFaceNormal(Ray ray, float3 outward_normal)
    {
        front_face = dot(ray.direction, outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

float rand(float seed)
{
    return frac(sin(seed * 12.9898) * 43758.5453);
}