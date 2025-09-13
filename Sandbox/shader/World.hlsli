#pragma once

#include "Sphere.hlsli"

[[vk::push_constant]]
struct PushConstants {
    uint viewport_width;
    uint viewport_height;
    uint sphere_count;
    uint samples_per_pixel;
    uint max_depth;
    float time;
} push_constants;

struct Camera
{
    float3 center;
    float _padding0;
    float3 pixel00_location;
    float _padding1;
    float3 pixel_delta_u;
    float _padding2;
    float3 pixel_delta_v;
    float _padding3;
};
[[vk::binding(1, 0)]]
ConstantBuffer<Camera> camera;

[[vk::binding(2, 0)]]
StructuredBuffer<Sphere> spheres;

class World : Object
{
    bool hit(Ray ray, Interval ray_t, out HitRecord rec)
    {
        bool is_hit = false;
        float closest_so_far = ray_t.max;

        [loop]
        for(int i=0; i<push_constants.sphere_count; i++)
        {
            HitRecord temp_rec;
            ray_t.max = closest_so_far;
            if(spheres[i].hit(ray, ray_t, temp_rec))
            {
                is_hit = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return is_hit;
    }
};