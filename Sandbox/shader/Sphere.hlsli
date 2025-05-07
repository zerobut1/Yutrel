#pragma once

#include "Object.hlsli"

class Sphere : Object
{
    float3 center;
    float radius;

    bool hit(Ray ray, Interval ray_t, out HitRecord rec)
    {
        float3 oc = center - ray.origin;
        float a = dot(ray.direction, ray.direction);
        float h = dot(ray.direction, oc);
        float c = dot(oc, oc) - radius * radius;

        float discriminant = h * h - a * c;

        [flatten]
        if(discriminant < 0)
        {
            return false;
        }
            
        float sqrtd = sqrt(discriminant);
        
        float root = (h - sqrtd) / a;
        [flatten]
        if(!ray_t.surrounds(root))
        {
            root = (h + sqrtd) / a;
            [flatten]
            if(!ray_t.surrounds(root))
            {
                return false;
            }
        }

        rec.t = root;
        rec.position = ray.at(rec.t);
        float3 outward_normal = (rec.position - center) / radius;
        rec.setFaceNormal(ray, outward_normal);

        return true;
    }
};