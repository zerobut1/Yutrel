#pragma once

#include "Common.hlsli"

enum MaterialType
{
    MT_None = 0,
    MT_Lambertian = 1,
    MT_Metal = 2,
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
        default:
            return false;
    }
    return false;
}