#version 460 core
layout(location = 0) out vec4 g_positon_depth;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec4 g_albedo;
layout(location = 3) out vec2 g_metallic_roughness;
layout(location = 4) out vec4 g_emissive;

in VS_OUT
{
    vec2 uv;
    vec3 world_position;
    vec3 world_normal;
    mat3 TBN;
}
fs_in;

// 材质
uniform vec4 u_albedo;
uniform float u_metallic;
uniform float u_roughness;
uniform vec3 u_emissive;

// toggle
uniform float u_normal_tex_toggle;
uniform float u_albedo_tex_toggle;
uniform float u_metallic_roughness_tex_toggle;

// PBR贴图
layout(binding = 1) uniform sampler2D u_normal_texture;
layout(binding = 2) uniform sampler2D u_albedo_texture;
layout(binding = 3) uniform sampler2D u_metallic_roughness_texture;

const float NEAR = 0.1; // 投影矩阵的近平面
const float FAR = 100.0; // 投影矩阵的远平面
//获取线性深度值
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main()
{    
    // 世界空间坐标
    g_positon_depth.xyz = fs_in.world_position;

    // 线性深度 介于near和far之间
    g_positon_depth.w = LinearizeDepth(gl_FragCoord.z);

    // 世界空间法向量
    vec3 tangent_normal = texture(u_normal_texture, fs_in.uv).rgb;
    g_normal = u_normal_tex_toggle > 0.5 ? fs_in.TBN * normalize(tangent_normal * 2.0 - 1.0) : fs_in.world_normal;

    // 基础色 / 反照率 / 漫反射颜色
    g_albedo = (u_albedo_tex_toggle > 0.5) ? texture(u_albedo_texture, fs_in.uv) : u_albedo;

    // 金属度 & 粗糙度
    g_metallic_roughness.r = u_metallic_roughness_tex_toggle > 0.5 ? texture(u_metallic_roughness_texture, fs_in.uv).b : u_metallic;
    g_metallic_roughness.g = u_metallic_roughness_tex_toggle > 0.5 ? texture(u_metallic_roughness_texture, fs_in.uv).g : u_roughness;

    // 自发光
    g_emissive = vec4(u_emissive, 1.0);
}