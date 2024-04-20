#version 450

layout(location = 0) out vec4 out_frag_color;

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec2 in_screen_uv;
layout(location = 3) in vec3 in_normal;
layout(location = 4) in vec4 in_tangent;
layout(location = 5) in float in_time;

layout(set = 0, binding = 0) uniform SceneData
{
    mat4 view;
    mat4 projection;
    vec3 camera_position;
    vec4 directional_light_color;
    vec3 directional_light_direction;
    float near;
    float far;
}
u_scene_data;

layout(set = 0, binding = 1) uniform sampler2D u_normal_texture;
layout(set = 0, binding = 2) uniform sampler2D u_brdf_lut;
layout(set = 0, binding = 3) uniform samplerCube u_prefiltered_map;
layout(set = 0, binding = 4) uniform samplerCube u_irradiance_map;
layout(set = 0, binding = 5) uniform sampler2D u_scene_color;
layout(set = 0, binding = 6) uniform sampler2D u_scene_depth;

//-------------计算反射-----------------------
#define PI 3.1415926535897932384626433832795

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

vec3 PrefilteredReflection(vec3 R, float roughness)
{
    const float MAX_REFLECTION_LOD = 9.0; // todo: param/const

    float lod  = roughness * MAX_REFLECTION_LOD;
    float lodf = floor(lod);
    float lodc = ceil(lod);
    vec3 a     = textureLod(u_prefiltered_map, R, lodf).rgb;
    vec3 b     = textureLod(u_prefiltered_map, R, lodc).rgb;
    return mix(a, b, lod - lodf);
}

#define BASE_COLOR vec3(0.0, 0.3, 1.0)

vec3 SpecularContribution(vec3 L, vec3 V, vec3 N, vec3 F0, float metallic, float roughness)
{
    // 预计算
    vec3 H      = normalize(V + L);
    float dotNV = clamp(dot(N, V), 0.0, 1.0);
    float dotNL = clamp(dot(N, L), 0.0, 1.0);
    float dotNH = clamp(dot(N, H), 0.0, 1.0);

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
        // color += spec * dotNL;
        color += (kD * BASE_COLOR / PI + spec) * dotNL;
    }

    return color;
}

vec3 CalculateNormal()
{
    // vec3 tangent_normal = texture(u_normal_texture, in_uv).xyz * 2.0 - 1.0;
    vec3 tangent_normal = vec3(0.0, 0.0, 1.0);

    vec3 N   = normalize(in_normal);
    vec3 T   = normalize(in_tangent.xyz);
    vec3 B   = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangent_normal);
}

vec3 Uncharted2Tonemap(vec3 x)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

float GetLinerSceneDepth(float scene_depth, float near, float far)
{
    return near * far / ((far - near) * scene_depth + near);
}

vec3 SampleNormal()
{
    // vec2 base_uv   = in_position.xz * 0.01 + mod(in_time * 0.00003, 1.0);
    vec2 base_uv   = in_position.xz * 0.1;
    vec2 offset_uv = vec2(0.0, 0.3);
    vec3 normal_1  = texture(u_normal_texture, base_uv + offset_uv).xzy * 2.0 - 1.0;
    vec3 normal_2  = texture(u_normal_texture, base_uv + vec2(0.5, 0.0) - offset_uv).xzy * 2.0 - 1.0;
    vec3 normal_3  = texture(u_normal_texture, base_uv.yx + offset_uv).xzy * 2.0 - 1.0;
    vec3 normal_4  = texture(u_normal_texture, base_uv.yx + vec2(0.5, 0.0) - offset_uv).xzy * 2.0 - 1.0;
    // return normalize(normal_1 + normal_2 + normal_3 + normal_4);
    return normalize(normal_1);
}

void main()
{
    vec3 base_color = BASE_COLOR;
    // vec3 world_normal = CalculateNormal();
    vec3 world_normal   = SampleNormal();
    vec3 world_position = in_position.xyz;
    float metallic      = 0.0;
    float roughness     = 0.0;

    vec3 scene_color  = texture(u_scene_color, in_screen_uv).rgb;
    float scene_depth = texture(u_scene_depth, in_screen_uv).r;

    //-------------PBR-------------
    vec3 N = normalize(world_normal);
    vec3 V = normalize(u_scene_data.camera_position - world_position);
    vec3 R = normalize(reflect(-V, N));

    // 基础反射率
    vec3 F0 = vec3(0.15);

    // 反射光
    vec3 Lo = vec3(0.0);
    // 平行光
    vec3 L = -normalize(u_scene_data.directional_light_direction);
    Lo += SpecularContribution(L, V, N, F0, metallic, roughness);

    // ibl
    vec2 brdf       = texture(u_brdf_lut, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 reflection = PrefilteredReflection(R, roughness).rgb;
    vec3 irradiance = texture(u_irradiance_map, N).rgb;

    vec3 diffuse = irradiance * base_color;

    vec3 F = F_SchlickR(max(dot(N, V), 0.0), F0, roughness);

    vec3 specular = reflection * (F * brdf.x + brdf.y);

    vec3 kD = 1.0 - F;
    kD *= 1.0 - metallic;
    vec3 ambient = (kD * diffuse + specular);

    // vec3 reflect_color = ambient;
    vec3 reflect_color = ambient + Lo;
    // vec3 reflect_color = reflection * base_color;

    vec3 color = vec3(1.0);

    float view_scene_depth = GetLinerSceneDepth(scene_depth, u_scene_data.near, u_scene_data.far);
    float view_depth       = GetLinerSceneDepth(gl_FragCoord.z, u_scene_data.near, u_scene_data.far);

    float water_depth = view_scene_depth - view_depth;
    water_depth       = clamp(water_depth, 0.0, 20.0) * 0.05;
    float fresnel     = (0.8 + 0.2 * F.x);
    // fresnel       = (fresnel - 0.5) * 4;

    scene_color = mix(scene_color, BASE_COLOR, water_depth);

    color = mix(scene_color, reflect_color, fresnel);
    // color = vec3(fresnel);

    // // tone mapping
    color = Uncharted2Tonemap(color * 4.5);
    color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));

    // // 伽马矫正
    color = pow(color, vec3(0.4545));

    // out_frag_color.rgb = color;
    // out_frag_color.rgb = vec3(0.4, 0.8, 1.0);
    out_frag_color.rgb = world_normal;
    // out_frag_color.rgb = vec3(water_depth);
    // out_frag_color.rgb = vec3(scene_depth);
    // out_frag_color.rgb = vec3(0.0, 1.0, 0.0);
    // out_frag_color.rgb = F;

    // out_frag_color.a = 0.8 + 0.2 * F.x;
    out_frag_color.a = 1.0;
}