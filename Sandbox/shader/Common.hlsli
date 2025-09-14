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

class Material
{
    uint type;
    uint ptr;
};

struct HitRecord
{
    float3 position;
    float3 normal;
    Material material;
    float t;
    bool front_face;

    void setFaceNormal(Ray ray, float3 outward_normal)
    {
        front_face = dot(ray.direction, outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

float random(float seed)
{
    return frac(sin(seed * 12.9898) * 43758.5453);
}

float3 randomFloat3(float min,float max, float seed)
{
    return lerp(min,
                max,
                frac(sin(dot(seed, float3(127.1, 311.7, 74.7))) * float3(43758.5453123, 78.233, 12.345)));
}

float3 randomUnitVector(float seed)
{
    return normalize(randomFloat3(-1, 1, seed));
}

float3 randomOnHemisphere(float3 normal, float seed)
{
    float3 on_unit_sphere = randomUnitVector(seed + dot(normal, float3(12.9898, 78.233, 45.164)));
    if(dot(on_unit_sphere, normal) > 0.0)
    {
        return on_unit_sphere;
    }
    else
    {
        return -on_unit_sphere;
    }
}

bool nearZero(float3 v)
{
    const float s = 1e-8;
    return (abs(v.x) < s) && (abs(v.y) < s) && (abs(v.z) < s);
}