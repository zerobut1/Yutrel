#include "World.hlsli"

RWTexture2D<float4> output_texture;

[numthreads(16, 16, 1)]
void main(
	int3 dispatch_thread_id : SV_DispatchThreadID,
	int3 group_thread_id : SV_GroupThreadID
	)
{
	int2 screen_position = int2(dispatch_thread_id.xy);

    float4 out_color = float4(0, 0, 0, 1); 

    for(int sample_index=0; sample_index < push_constants.samples_per_pixel; sample_index++)
    {
        float2 pixel_offset = float2(rand(screen_position.x + screen_position.y + sample_index + push_constants.time), rand(screen_position.x + screen_position.y + sample_index + push_constants.time));
        pixel_offset = (pixel_offset - 0.5) / float2(push_constants.viewport_width, push_constants.viewport_height);

        Ray ray;
        ray.origin = camera.center;
        ray.direction = camera.pixel00_location + camera.pixel_delta_u * screen_position.x + camera.pixel_delta_v * screen_position.y + float3(pixel_offset, 0) - camera.center;

        World world;
        HitRecord rec;
        Interval ray_t;
        ray_t.min = 0;
        ray_t.max = POSITIVE_INFINITY;

        [flatten]
        if(world.hit(ray, ray_t, rec))
        {
            out_color.rgb += 0.5 * (rec.normal + float3(1, 1, 1));
        }
        else
        {
            float3 direction = normalize(ray.direction);
            float t = 0.5 * (direction.y + 1.0);
            out_color.rgb += (1.0 - t) * float3(1.0, 1.0, 1.0) + t * float3(0.4, 0.8, 1.0);
        }
    }

	output_texture[dispatch_thread_id.xy] = saturate(out_color/(float)push_constants.samples_per_pixel);
}