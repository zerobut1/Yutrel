#version 450

layout(location = 0) out vec4 out_base_color;
layout(location = 1) out vec4 out_world_normal;
layout(location = 2) out vec4 out_world_position;
layout(location = 3) out vec4 out_metallic_roughness;

layout(location = 0) in vec3 in_world_pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec4 in_tangent;
layout(location = 3) in vec2 in_uv;

layout(set = 1, binding = 0) uniform MaterialData
{
    vec4 base_color;
}
u_mat_data;
layout(set = 1, binding = 1) uniform sampler2D u_base_color_texture;
layout(set = 1, binding = 2) uniform sampler2D u_metallic_roughness_texture;
layout(set = 1, binding = 3) uniform sampler2D u_normal_texture;

#define ALBEDO pow(texture(u_base_color_texture, in_uv).rgb, vec3(2.2))

vec3 CalculateNormal()
{
    vec3 tangent_normal = texture(u_normal_texture, in_uv).xyz * 2.0 - 1.0;

    vec3 N   = normalize(in_normal);
    vec3 T   = normalize(in_tangent.xyz);
    vec3 B   = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangent_normal);
}

void main()
{
    out_base_color = vec4(ALBEDO, 1.0);

    out_world_normal = vec4(CalculateNormal(), 1.0);

    out_world_position = vec4(in_world_pos, 1.0);

    float metallic         = texture(u_metallic_roughness_texture, in_uv).b;
    float roughness        = texture(u_metallic_roughness_texture, in_uv).g;
    out_metallic_roughness = vec4(metallic, roughness, 1.0, 0.0);
}