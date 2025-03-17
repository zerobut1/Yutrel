#include "Common.hlsl"

RWTexture2D<float4> output_texture;

[[vk::push_constant]]
struct PushConstants {
    uint viewport_width;
    uint viewport_height;
} push_constant;

float HitSphere(in Ray ray, float3 center, float radius)
{
    float3 co = ray.origin - center;
    float a = dot(ray.direction, ray.direction);
    float half_b = dot(co, ray.direction);
    float c = dot(co, co) - radius * radius;

    float discriminant = half_b * half_b - a * c;

    [flatten]
    if(discriminant < 0)
    {
        return -1.0f;
    }
    else
    {
        return (-half_b - sqrt(discriminant)) / a;
    }
}

float3 RayColor(in Ray ray) 
{
    float3 sphere_center = float3(0, 0, -1.0f);
    float sphere_radius = 0.5f;
    float t = HitSphere(ray, sphere_center, sphere_radius);
    [flatten]
    if(t > 0.0f)
    {
        float3 N = normalize(RayAt(ray, t) - sphere_center);
        return N;
    }

    float3 direction = normalize(ray.direction);
    t = 0.5 * (direction.y + 1.0);
    return (1.0 - t) * float3(1.0, 1.0, 1.0) + t * float3(0.4, 0.8, 1.0);
}

[numthreads(16, 16, 1)]
void main(
	int3 dispatch_thread_id : SV_DispatchThreadID,
	int3 group_thread_id : SV_GroupThreadID
	)
{
	int2 screen_position = int2(dispatch_thread_id.xy);
	float width = push_constant.viewport_width;
    float height = push_constant.viewport_height;

    float3 origin = float3(0, 0, 0);
    float3 vertical = float3(0, -2.0, 0);
    float3 horizontal = float3((width / height) * 2.0, 0, 0);
    float3 upper_left_corner = float3(-(width / height), 1.0, -1.0);

    {
        float u = float(screen_position.x) / width;
        float v = float(screen_position.y) / height;

        float4 color = float4(0.0, 0.0, 0.0, 1.0);

        Ray ray;
        ray.origin = origin;
        ray.direction = upper_left_corner + u * horizontal + v * vertical;

        color.xyz = RayColor(ray);
		
		output_texture[dispatch_thread_id.xy] = color;
    }
}