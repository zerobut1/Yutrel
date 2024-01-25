#version 450

layout(location = 0)in vec2 in_uv;
layout(location = 1)in vec3 in_normal;
layout(location = 2)in vec4 in_tangent;
layout(location = 3)in vec3 in_view_vec;

layout(location = 0)out vec4 out_frag_color;

layout(set = 0, binding = 0)uniform MaterialData {
    vec4 base_color;
} u_mat_data;
layout(set = 0, binding = 1)uniform sampler2D u_base_color_texture;
layout(set = 0, binding = 2)uniform sampler2D u_metallic_roughness_texture;
layout(set = 0, binding = 3)uniform sampler2D u_normal_texture;

layout(set = 1, binding = 0)uniform SceneData {
    mat4 view;
    mat4 proj;
    mat4 view_proj;
    vec4 view_position;
    vec4 ambient_color;
    vec4 sunlight_direction;
    vec4 sunlight_color;
} u_scene_data;

void main()
{
    vec4 color = texture(u_base_color_texture, in_uv);
    
    vec3 N = normalize(in_normal);
    vec3 T = normalize(in_tangent.xyz);
    vec3 B = cross(in_normal, in_tangent.xyz) * in_tangent.w;
    // vec3 B = cross(in_normal, in_tangent.xyz);
    mat3 TBN = mat3(T, B, N);
    N = TBN * normalize(texture(u_normal_texture, in_uv).xyz * 2.0 - vec3(1.0));
    
    const float ambient = 0.1;
    vec3 L = normalize(u_scene_data.sunlight_direction.xyz);
    vec3 V = normalize(in_view_vec);
    vec3 R = reflect(-L, N);
    vec3 diffuse = max(dot(N, L), ambient).rrr;
    float specular = pow(max(dot(R, V), 0.0), 32.0);
    // out_frag_color = vec4(diffuse * color.rgb + specular, color.a);
    out_frag_color = texture(u_metallic_roughness_texture, in_uv);
}