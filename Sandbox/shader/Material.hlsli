#pragma once

#include "Common.hlsli"

[[vk::binding(2, 0)]]
StructuredBuffer<float4> material_data;

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
    
    bool scatter(Ray ray_in, HitRecord rec, out float3 attenuation, out Ray scattered, float seed)
    {
        float3 reflected = reflect(normalize(ray_in.direction), rec.normal);
        scattered.origin = rec.position;
        scattered.direction = normalize(reflected);
        attenuation = albedo;
        return true;
    }
};

bool scatter(Material material, Ray ray_in, HitRecord rec, out float3 attenuation, in out Ray scattered, float seed)
{
    switch(material.type)
    {
        case 1:
        {
            Lambertian lam;
            lam.albedo = material_data[material.ptr].xyz;
            return lam.scatter(ray_in, rec, attenuation, scattered, seed);
        }
        case 2:
        {
            Metal met;
            met.albedo = material_data[material.ptr].xyz;
            return met.scatter(ray_in, rec, attenuation, scattered, seed);
        }
    }
    return false;
}