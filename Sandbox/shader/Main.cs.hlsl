#include "World.hlsli"

RWTexture2D<float4> output_texture;

float3 backgroundColor(Ray ray)
{
    float3 direction = normalize(ray.direction);
    float t = 0.5 * (direction.y + 1.0);
    return (1.0 - t) * float3(1.0, 1.0, 1.0) + t * float3(0.4, 0.8, 1.0);
}

float3 rayColor(Ray ray, World world, float seed)
{
    float3 out_color = float3(1, 1, 1);

    HitRecord rec;
    Interval ray_t;
    ray_t.min = 0.001;
    ray_t.max = POSITIVE_INFINITY;

    for(int depth = 0; depth < push_constants.max_depth; depth++)
    {
        [flatten]
        if(world.hit(ray, ray_t, rec))
        {
            float3 direction = randomOnHemisphere(rec.normal, seed + depth);
            ray.origin = rec.position;
            ray.direction = direction;
            out_color *= 0.5;
        }
        else
        {
            return out_color * backgroundColor(ray);
        }
    }

    return out_color;
}

[numthreads(16, 16, 1)]
void main(
	int3 dispatch_thread_id : SV_DispatchThreadID,
	int3 group_thread_id : SV_GroupThreadID
	)
{
	int2 screen_position = int2(dispatch_thread_id.xy);

    float4 out_color = float4(0, 0, 0, 1); 

    World world;

    for(int sample_index = 0; sample_index < push_constants.samples_per_pixel; sample_index++)
    {
        float seed = sample_index + push_constants.time;

        float2 pixel_offset = float2(
                                random(seed),
                                random(seed));
        pixel_offset = (pixel_offset - 0.5) / float2(push_constants.viewport_width, push_constants.viewport_height);

        Ray ray;
        ray.origin = camera.center;
        ray.direction = camera.pixel00_location +
                        camera.pixel_delta_u * screen_position.x +
                        camera.pixel_delta_v * screen_position.y +
                        float3(pixel_offset, 0) -
                        camera.center;

        out_color.rgb += rayColor(ray, world, seed);
    }

	output_texture[dispatch_thread_id.xy] = saturate(out_color / (float)push_constants.samples_per_pixel);
}