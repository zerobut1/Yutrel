#include "World.hlsli"

RWTexture2D<float4> output_texture;

[numthreads(16, 16, 1)]
void main(
	int3 dispatch_thread_id : SV_DispatchThreadID,
	int3 group_thread_id : SV_GroupThreadID
	)
{
	int2 screen_position = int2(dispatch_thread_id.xy);

    Ray ray;
    ray.origin = camera.center;
    ray.direction = camera.pixel00_location + camera.pixel_delta_u * screen_position.x + camera.pixel_delta_v * screen_position.y - camera.center;

    float4 out_color = float4(0, 0, 0, 1);

    World world;
    HitRecord rec;
    Interval ray_t;
    ray_t.min = 0;
    ray_t.max = POSITIVE_INFINITY;

    [flatten]
    if(world.hit(ray, ray_t, rec))
    {
        out_color.rgb = 0.5 * (rec.normal + float3(1, 1, 1));
    }
    else
    {
        float3 direction = normalize(ray.direction);
        float t = 0.5 * (direction.y + 1.0);
        out_color.rgb = (1.0 - t) * float3(1.0, 1.0, 1.0) + t * float3(0.4, 0.8, 1.0);
    }

	output_texture[dispatch_thread_id.xy] = out_color;
}