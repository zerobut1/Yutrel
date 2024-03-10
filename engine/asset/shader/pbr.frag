#version 450

layout(location = 0) in vec3 in_world_pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec4 in_tangent;
layout(location = 3) in vec2 in_uv;
layout(location = 4) in vec4 in_view_space_pos;

layout(location = 0) out vec4 out_frag_color;

#define SHADOWMAP_CASCADE_COUNT 4

layout(set = 0, binding = 0) uniform SceneData
{
    mat4 view;
    mat4 projection;
    vec3 camera_position;
    vec4 ambient_color;
    vec4 directional_light_color;
    vec3 directional_light_direction;
    // todo 使用数组
    vec4 cascade_splits;
    mat4 directional_light_VP[SHADOWMAP_CASCADE_COUNT];
}
u_scene_data;
layout(set = 0, binding = 1) uniform sampler2DArray u_directional_light_shadowmap;

layout(set = 1, binding = 0) uniform MaterialData
{
    vec4 base_color;
}
u_mat_data;
layout(set = 1, binding = 1) uniform sampler2D u_base_color_texture;
layout(set = 1, binding = 2) uniform sampler2D u_metallic_roughness_texture;
layout(set = 1, binding = 3) uniform sampler2D u_normal_texture;

//-------------shadowmap------------
float TextureProj(vec4 shadow_coord, vec2 offset, uint cascade_index)
{
    float shadow = 1.0;
    float bias   = 0.0001;

    if (shadow_coord.z > 0.0 && shadow_coord.z < 1.0)
    {
        float dist = texture(u_directional_light_shadowmap, vec3(shadow_coord.xy + offset, cascade_index)).r;
        if (shadow_coord.w > 0.0 && dist > shadow_coord.z + bias)
        {
            shadow = 0.0;
        }
    }
    return shadow;
}

float FilterPCF(vec4 shadow_coord, uint cascade_index)
{
    ivec2 tex_dim = textureSize(u_directional_light_shadowmap, 0).xy;
    float scale   = 0.75;
    float dx      = scale * 1.0 / float(tex_dim.x);
    float dy      = scale * 1.0 / float(tex_dim.y);

    float shadow_factor = 0.0;
    int count           = 0;
    int range           = 1;

    for (int x = -range; x <= range; x++)
    {
        for (int y = -range; y <= range; y++)
        {
            shadow_factor += TextureProj(shadow_coord, vec2(dx * x, dy * y), cascade_index);
            count++;
        }
    }
    return shadow_factor / count;
}

const mat4 bias_mat = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.5, 0.5, 0.0, 1.0);

//--------------根据法线贴图计算法线-------------
vec3 CalculateNormal()
{
    vec3 tangent_normal = texture(u_normal_texture, in_uv).xyz * 2.0 - 1.0;

    vec3 N   = normalize(in_normal);
    vec3 T   = normalize(in_tangent.xyz);
    vec3 B   = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangent_normal);
}

//-------------计算反射-----------------------
#define PI 3.1415926535897932384626433832795
#define ALBEDO pow(texture(u_base_color_texture, in_uv).rgb, vec3(2.2))
// #define ALBEDO texture(u_base_color_texture, in_uv).rgb

// 法线分布函数
float D_GGX(float dotNH, float roughness)
{
    float alpha  = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom  = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
    return (alpha2) / (PI * denom * denom);
}

// 几何遮蔽函数
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
    float r  = (roughness + 1.0);
    float k  = (r * r) / 8.0;
    float GL = dotNL / (dotNL * (1.0 - k) + k);
    float GV = dotNV / (dotNV * (1.0 - k) + k);
    return GL * GV;
}

// 菲涅尔项
vec3 F_Schlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 F_SchlickR(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 SpecularContribution(vec3 L, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
    // 预计算
    vec3 H      = normalize(V + L);
    float dotNV = clamp(dot(N, V), 0.0, 1.0);
    float dotNL = clamp(dot(N, L), 0.0, 1.0);
    float dotLH = clamp(dot(L, H), 0.0, 1.0);
    float dotNH = clamp(dot(N, H), 0.0, 1.0);

    vec3 light_color = vec3(1.0);

    vec3 color = vec3(0.0);

    if (dotNL > 0.0)
    {
        // D = Normal distribution (Distribution of the microfacets)
        float D = D_GGX(dotNH, roughness);
        // G = Geometric shadowing term (Microfacets shadowing)
        float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
        // F = Fresnel factor (Reflectance depending on angle of incidence)
        vec3 F    = F_Schlick(dotNV, F0);
        vec3 spec = D * F * G / (4.0 * dotNL * dotNV + 0.001);
        vec3 kD   = (vec3(1.0) - F) * (1.0 - metallic);
        color += (kD * ALBEDO / PI + spec) * dotNL;
    }

    // if (dotNL > 0.0)
    // {
    //     float rroughness = max(0.05, roughness);
    //     // D = Normal distribution (Distribution of the microfacets)
    //     float D = D_GGX(dotNH, roughness);
    //     // G = Geometric shadowing term (Microfacets shadowing)
    //     float G = G_SchlicksmithGGX(dotNL, dotNV, rroughness);
    //     // F = Fresnel factor (Reflectance depending on angle of incidence)
    //     vec3 F = F_Schlick(dotNV, F0);

    //     vec3 spec = D * F * G / (4.0 * dotNL * dotNV);

    //     color += spec * dotNL * light_color;
    // }

    return color;
}

void main()
{
    //-------------阴影----------------
    // 计算应该在哪个级联采样
    uint cascade_index = 0;
    for (uint i = 0; i < SHADOWMAP_CASCADE_COUNT - 1; i++)
    {
        if (in_view_space_pos.z < u_scene_data.cascade_splits[i])
        {
            cascade_index = i + 1;
        }
    }

    vec4 shadow_coord = (bias_mat * u_scene_data.directional_light_VP[cascade_index]) * vec4(in_world_pos, 1.0f);

    float shadow = FilterPCF(shadow_coord / shadow_coord.w, cascade_index);

    //-------------PBR-------------
    vec3 N = CalculateNormal();
    vec3 V = normalize(u_scene_data.camera_position - in_world_pos);
    vec3 R = reflect(-V, N);

    vec3 albedo     = texture(u_base_color_texture, in_uv).rgb;
    float metallic  = texture(u_metallic_roughness_texture, in_uv).b;
    float roughness = texture(u_metallic_roughness_texture, in_uv).g;

    // 菲涅尔项
    vec3 F0 = vec3(0.04);
    F0      = mix(F0, albedo.rgb, metallic);

    // 反射光
    vec3 Lo = vec3(0.0);
    // 平行光
    vec3 L = -normalize(u_scene_data.directional_light_direction);
    Lo += SpecularContribution(L, V, N, F0, metallic, roughness) * shadow;

    const float ambient = 0.02;

    vec3 color = ALBEDO * ambient;
    color += Lo;

    // 色调映射
    // color = color / (color + vec3(1.0));
    // 伽马矫正
    color = pow(color, vec3(0.4545));

    out_frag_color = vec4(color, 1.0);
}