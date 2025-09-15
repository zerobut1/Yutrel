#pragma once

#include "Sphere.hlsli"
#include "Camera.hlsli"

[[vk::push_constant]]
struct PushConstants {
    uint viewport_width;
    uint viewport_height;
    uint sphere_count;
    float time;
} push_constants;

[[vk::binding(3, 0)]]
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