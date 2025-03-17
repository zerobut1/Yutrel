#include "Common.hlsl"

RWTexture2D<float4> output_texture;

[[vk::push_constant]]
struct PushConstants {
    uint viewport_width;
    uint viewport_height;
} push_constant;


float3 RayColor(in Ray ray) 
{
    float3 direction = normalize(ray.direction);
    float t = 0.5 * (direction.y + 1.0);
    return (1.0-t) * float3(1.0, 1.0, 1.0) + t * float3(0.4, 0.8, 1.0);
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