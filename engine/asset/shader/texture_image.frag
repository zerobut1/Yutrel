#version 450

layout(location = 0)in vec2 in_uv;
layout(location = 1)in vec3 in_normal;
layout(location = 2)in vec4 in_tangent;
layout(location = 3)in vec3 in_view_vec;
layout(location = 4)in vec4 in_directional_light_shadow_coord;

layout(location = 0)out vec4 out_frag_color;

layout(set = 0, binding = 0)uniform SceneData {
    mat4 view;
    mat4 projection;
    vec3 camera_position;
    vec4 ambient_color;
    vec4 directional_light_color;
    vec3 directional_light_direction;
    mat4 directional_light_VP;
} u_scene_data;
layout(set = 0, binding = 1)uniform sampler2D u_directional_light_shadowmap;

layout(set = 1, binding = 0)uniform MaterialData {
    vec4 base_color;
} u_mat_data;
layout(set = 1, binding = 1)uniform sampler2D u_base_color_texture;
layout(set = 1, binding = 2)uniform sampler2D u_metallic_roughness_texture;
layout(set = 1, binding = 3)uniform sampler2D u_normal_texture;

const float bias = -0.0001;

float TextureProj(vec4 shadow_coord, vec2 off)
{
    float shadow = 1.0;
    if (shadow_coord.z > 0.0 && shadow_coord.z < 1.0)
    {
        float dist = texture(u_directional_light_shadowmap, shadow_coord.xy + off).r + bias;
        if (shadow_coord.w > 0.0 && dist > shadow_coord.z)
        {
            shadow = 0.1;
        }
    }
    return shadow;
}

void main()
{
    float shadow = TextureProj(in_directional_light_shadow_coord / in_directional_light_shadow_coord.w, vec2(0.0f));
    
    vec4 color = texture(u_base_color_texture, in_uv);
    
    vec3 N = normalize(in_normal);
    vec3 T = normalize(in_tangent.xyz);
    vec3 B = cross(in_normal, in_tangent.xyz) * in_tangent.w;
    mat3 TBN = mat3(T, B, N);
    N = TBN * normalize(texture(u_normal_texture, in_uv).xyz * 2.0 - vec3(1.0));
    
    const float ambient = 0.1;
    vec3 L = -normalize(u_scene_data.directional_light_direction);
    vec3 V = normalize(in_view_vec);
    vec3 R = reflect(-L, N);
    vec3 diffuse = max(dot(N, L), ambient).rrr;
    float specular = pow(max(dot(R, V), 0.0), 32.0);
    out_frag_color = vec4((diffuse * color.rgb + specular) * shadow, color.a);
}