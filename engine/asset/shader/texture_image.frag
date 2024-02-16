#version 450

layout(location = 0)in vec2 in_uv;
layout(location = 1)in vec3 in_normal;
layout(location = 2)in vec4 in_tangent;
layout(location = 3)in vec3 in_view_vec;
layout(location = 4)in vec4 in_world_space_pos;
layout(location = 5)in vec4 in_view_space_pos;

layout(location = 0)out vec4 out_frag_color;

#define SHADOWMAP_CASCADE_COUNT 4

layout(set = 0, binding = 0)uniform SceneData {
    mat4 view;
    mat4 projection;
    vec3 camera_position;
    vec4 ambient_color;
    vec4 directional_light_color;
    vec3 directional_light_direction;
    // todo 使用数组
    vec4 cascade_splits;
    mat4 directional_light_VP[SHADOWMAP_CASCADE_COUNT];
} u_scene_data;
layout(set = 0, binding = 1)uniform sampler2DArray u_directional_light_shadowmap;

layout(set = 1, binding = 0)uniform MaterialData {
    vec4 base_color;
} u_mat_data;
layout(set = 1, binding = 1)uniform sampler2D u_base_color_texture;
layout(set = 1, binding = 2)uniform sampler2D u_metallic_roughness_texture;
layout(set = 1, binding = 3)uniform sampler2D u_normal_texture;

float TextureProj(vec4 shadow_coord, vec2 offset, uint cascade_index)
{
    float shadow = 1.0;
    float bias = 0.0001;
    
    if (shadow_coord.z > 0.0 && shadow_coord.z < 1.0)
    {
        float dist = texture(u_directional_light_shadowmap, vec3(shadow_coord.xy + offset, cascade_index)).r;
        if (shadow_coord.w > 0.0 && dist > shadow_coord.z + bias)
        {
            shadow = 0.1;
        }
    }
    return shadow;
}

float FilterPCF(vec4 shadow_coord, uint cascade_index)
{
    ivec2 tex_dim = textureSize(u_directional_light_shadowmap, 0).xy;
    float scale = 0.75;
    float dx = scale * 1.0 / float(tex_dim.x);
    float dy = scale * 1.0 / float(tex_dim.y);
    
    float shadow_factor = 0.0;
    int count = 0;
    int range = 1;
    
    for(int x = -range; x <= range; x ++ ) {
        for(int y = -range; y <= range; y ++ ) {
            shadow_factor += TextureProj(shadow_coord, vec2(dx * x, dy * y), cascade_index);
            count ++ ;
        }
    }
    return shadow_factor / count;
}

const mat4 bias_mat = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 1.0
);

void main()
{
    // 计算应该在哪个级联采样
    uint cascade_index = 0;
    for(uint i = 0; i < SHADOWMAP_CASCADE_COUNT - 1; i ++ ) {
        if (in_view_space_pos.z < u_scene_data.cascade_splits[i]) {
            cascade_index = i+1;
        }
    }
    
    vec4 shadow_coord = (bias_mat * u_scene_data.directional_light_VP[cascade_index]) * in_world_space_pos;
    
    float shadow = FilterPCF(shadow_coord / shadow_coord.w, cascade_index);
    
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
    out_frag_color = vec4(diffuse * color.rgb + specular, color.a);
    out_frag_color *= shadow;
    
    // switch(cascade_index) {
        //     case 0 :
        //     out_frag_color.rgb = vec3(1.0f, 0.25f, 0.25f);
        //     break;
        //     case 1 :
        //     out_frag_color.rgb = vec3(0.25f, 1.0f, 0.25f);
        //     break;
        //     case 2 :
        //     out_frag_color.rgb = vec3(0.25f, 0.25f, 1.0f);
        //     break;
        //     case 3 :
        //     out_frag_color.rgb = vec3(1.0f, 1.0f, 0.25f);
        //     break;
    // }
    
    // out_frag_color = shadow_coord / shadow_coord.w;
}