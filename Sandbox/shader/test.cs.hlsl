#include "Sphere.hlsl"

RWTexture2D<float4> output_texture;

[[vk::push_constant]]
struct PushConstants {
    uint viewport_width;
    uint viewport_height;
    uint sphere_count;
} push_constants;

[[vk::binding(1, 0)]]
StructuredBuffer<Sphere> spheres;

[numthreads(16, 16, 1)]
void main(
	int3 dispatch_thread_id : SV_DispatchThreadID,
	int3 group_thread_id : SV_GroupThreadID
	)
{
	int2 screen_position = int2(dispatch_thread_id.xy);
	float width = push_constants.viewport_width;
    float height = push_constants.viewport_height;
    float u = float(screen_position.x) / width;
    float v = float(screen_position.y) / height;

    float3 origin = float3(0, 0, 0);
    float3 vertical = float3(0, -2.0f, 0);
    float3 horizontal = float3((width / height) * 2.0f, 0, 0);
    float3 upper_left_corner = float3(-(width / height), 1.0f, -1.0f);

    Ray ray;
    ray.origin = origin;
    ray.direction = upper_left_corner + u * horizontal + v * vertical;

    float4 out_color = float4(0, 0, 0, 1);

    HitRecord rec;
    bool is_hit = false;
    float closest_so_far = POSITIVE_INFINITY;
    [unroll]
    for(int i=0; i<push_constants.sphere_count; i++)
    {
        HitRecord temp_rec;
        [flatten]
        if(spheres[i].hit(ray, 0, closest_so_far, temp_rec))
        {
            is_hit = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    [flatten]
    if(is_hit)
    {
        out_color.xyz = 0.5 * (rec.normal + float3(1, 1, 1));
    }
    else
    {
        float3 direction = normalize(ray.direction);
        float t = 0.5 * (direction.y + 1.0);
        out_color.xyz = (1.0 - t) * float3(1.0, 1.0, 1.0) + t * float3(0.4, 0.8, 1.0);
    }

	output_texture[dispatch_thread_id.xy] = out_color;
}