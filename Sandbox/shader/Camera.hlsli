#pragma once

#include "Common.hlsli"

struct Camera
{
    float3 center;
    uint samples_per_pixel;
    float3 pixel00_location;
    uint max_depth;
    float3 pixel_delta_u;
    float defocus_angle;
    float3 pixel_delta_v;
    float3 defocus_disk_u;
    float3 defocus_disk_v;
};
[[vk::binding(1, 0)]]
ConstantBuffer<Camera> camera;

float3 defocusDiskSample(float seed)
{
    float3 p = randomInUnitDisk(seed);
    return camera.center + (p.x * camera.defocus_disk_u) + (p.y * camera.defocus_disk_v);
}

Ray getRay(int2 screen_position, float seed)
{
    Ray ray;

    float3 offset = float3(random(seed + 783) - 0.5, random(seed + 21378) - 0.5, 0);
    float3 pixel_sample = camera.pixel00_location 
                        + ((screen_position.x + offset.x) * camera.pixel_delta_u)
                        + ((screen_position.y + offset.y) * camera.pixel_delta_v);

    ray.origin = camera.defocus_angle > 0 ? defocusDiskSample(seed) : camera.center;
    ray.direction = pixel_sample - ray.origin;
    return ray;
}