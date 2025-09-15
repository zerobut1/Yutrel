#pragma once

#include "Common.hlsli"

enum MaterialType
{
    MT_None = 0,
    MT_Lambertian = 1,
    MT_Metal = 2,
    MT_Dielectric = 3,
};

class Material
{
    MaterialType type;
    float4 data;
};

[[vk::binding(2, 0)]]
StructuredBuffer<Material> material_data;

class Lambertian
{
    float3 albedo;

    bool scatter(Ray ray_in, HitRecord rec, out float3 attenuation, out Ray scattered, float seed)
    {
        float3 scatter_direction = rec.normal + randomUnitVector(seed + + dot(rec.normal, float3(12.9898, 78.233, 45.164)));
       
        if(nearZero(scatter_direction))
        {
            scatter_direction = rec.normal;
        }
        scattered.origin = rec.position;
        scattered.direction = normalize(scatter_direction);
        attenuation = albedo;
        return true;
    }
};

class Metal
{
    float3 albedo;
    float fuzz;
    
    bool scatter(Ray ray_in, HitRecord rec, out float3 attenuation, out Ray scattered, float seed)
    {
        float3 reflected = reflect(normalize(ray_in.direction), rec.normal);
        reflected = normalize(reflected) + (saturate(fuzz) * randomUnitVector(seed + dot(reflected, float3(12.9898, 78.233, 45.164))));

        scattered.origin = rec.position;
        scattered.direction = normalize(reflected);
        attenuation = albedo;

        return (dot(scattered.direction, rec.normal) > 0);
    }
};

class Dielectric
{
    float refraction_index;

    bool scatter(Ray ray_in, HitRecord rec, out float3 attenuation, out Ray scattered, float seed)
    {
        attenuation = float3(1.0, 1.0, 1.0);
        float refraction_ratio = rec.front_face ? (1.0 / refraction_index) : refraction_index;

        float3 unit_direction = normalize(ray_in.direction);
        float cos_theta = min(dot(-unit_direction, rec.normal), 1.0);
        float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        float3 direction;

        if(cannot_refract || reflectance(cos_theta, refraction_ratio) > random(seed + dot(unit_direction, float3(12.9898, 78.233, 45.164))))
        {
            direction = reflect(unit_direction, rec.normal);
        }
        else
        {
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        }

        scattered.origin = rec.position;
        scattered.direction = normalize(direction);
        return true;
    }

    float reflectance(float cosine, float ref_idx)
    {
        // Use Schlick's approximation for reflectance.
        float r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

bool scatter(MaterialIndex material_index, Ray ray_in, HitRecord rec, out float3 attenuation, in out Ray scattered, float seed)
{
    Material material = material_data[material_index.index];
    switch(material.type)
    {
        case MT_Lambertian:
        {
            Lambertian lam;
            lam.albedo = material.data.xyz;
            return lam.scatter(ray_in, rec, attenuation, scattered, seed);
        }
        case MT_Metal:
        {
            Metal met;
            met.albedo = material.data.xyz;
            met.fuzz = material.data.w;
            return met.scatter(ray_in, rec, attenuation, scattered, seed);
        }
        case MT_Dielectric:
        {
            Dielectric die;
            die.refraction_index = material.data.x;
            return die.scatter(ray_in, rec, attenuation, scattered, seed);
        }
        default:
            return false;
    }
    return false;
}