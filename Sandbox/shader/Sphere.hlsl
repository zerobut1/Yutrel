#include "Object.hlsl"

class Sphere : Object
{
    float3 center;
    float radius;

    bool hit(Ray ray, float t_min, float t_max, out HitRecord rec)
    {
        float3 co = ray.origin - center;
        float a = dot(ray.direction, ray.direction);
        float half_b = dot(co, ray.direction);
        float c = dot(co, co) - radius * radius;

        float discriminant = half_b * half_b - a * c;

        [flatten]
        if(discriminant > 0)
        {
            float root = sqrt(discriminant);
            
            float temp = (-half_b - root) / a;
            [flatten]
            if(temp < t_max && temp > t_min)
            {
                rec.t = temp;
                rec.position = ray.at(rec.t);
                float3 outward_normal = (rec.position - center) / radius;
                rec.setFaceNormal(ray, outward_normal);
                return true;
            }
            
            temp = (-half_b + root) / a;
            if (temp < t_max && temp > t_min)
            {
                rec.t = temp;
                rec.position = ray.at(rec.t);
                float3 outward_normal = (rec.position - center) / radius;
                rec.setFaceNormal(ray, outward_normal);
                return true;
            }
        }
        
        return false;
    }
};