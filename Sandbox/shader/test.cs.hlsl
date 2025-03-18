#include "Common.hlsl"

RWTexture2D<float4> output_texture;

[[vk::push_constant]]
struct PushConstants {
    uint viewport_width;
    uint viewport_height;
} push_constant;

double HitSphere(in Ray ray, double3 center, double radius)
{
    double3 co = ray.origin - center;
    double a = dot(ray.direction, ray.direction);
    double half_b = dot(co, ray.direction);
    double c = dot(co, co) - radius * radius;

    double discriminant = half_b * half_b - a * c;

    [flatten]
    if(discriminant < 0)
    {
        return -1.0;
    }
    else
    {
        return (-half_b - sqrt(discriminant)) / a;
    }
}

double3 RayColor(in Ray ray) 
{
    double3 sphere_center = double3(0, 0, -1.0);
    double sphere_radius = 0.5;
    double t = HitSphere(ray, sphere_center, sphere_radius);
    [flatten]
    if(t > 0.0)
    {
        double3 N = normalize(ray.At(t) - sphere_center);
        return N;
    }

    double3 direction = normalize(ray.direction);
    t = 0.5 * (direction.y + 1.0);
    return (1.0 - t) * double3(1.0, 1.0, 1.0) + t * double3(0.4, 0.8, 1.0);
}

[numthreads(16, 16, 1)]
void main(
	uint3 dispatch_thread_id : SV_DispatchThreadID,
	uint3 group_thread_id : SV_GroupThreadID
	)
{
	double2 screen_position = double2(dispatch_thread_id.xy);
	double width = push_constant.viewport_width;
    double height = push_constant.viewport_height;
    double2 uv = screen_position / double2(width, height);

    double3 origin = double3(0, 0, 0);
    double3 vertical = double3(0, -2.0, 0);
    double3 horizontal = double3((width / height) * 2.0, 0, 0);
    double3 upper_left_corner = double3(-(width / height), 1.0, -1.0);

    {
        double4 color = double4(0.0, 0.0, 0.0, 1.0);

        Ray ray;
        ray.origin = origin;
        ray.direction = upper_left_corner + uv.x * horizontal + uv.y * vertical;

        color.xyz = RayColor(ray);
		
		output_texture[dispatch_thread_id.xy] = color;
    }
}