struct Ray
{
    float3 origin;
    float3 direction;
};

float3 RayAt(in Ray ray, in float t)
{
    return ray.origin + ray.direction * t;
}