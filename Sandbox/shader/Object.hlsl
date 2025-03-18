#include "Common.hlsl"

interface Object
{
    bool hit(Ray ray, float t_min, float t_max, out HitRecord rec);
};