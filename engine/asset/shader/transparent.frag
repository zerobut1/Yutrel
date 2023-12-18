#version 460 core
layout(location = 0) out vec4 accum;
layout(location = 1) out float reveal;

in VS_OUT
{
    vec2 uv;
    vec3 world_pos;
    vec3 tangent_world_pos;
    vec3 tangent_camera_pos;
    vec3 tangent_sun_direction;
    vec3 tangent_point_light_position;
    mat3 TBN;
}
fs_in;

layout(std140, binding = 0) uniform Camera
{
    vec4 position;
    mat4 view;
    mat4 projection;
}
camera;

layout(std140, binding = 1) uniform Sun
{
    vec4 color;
    vec4 direction;
    vec4 intensity;
    mat4 light_space_matrices[4];
}
sun;

//材质
uniform vec4 u_albedo;
uniform float u_metallic;
uniform float u_roughness;
uniform vec3 u_emissive;

// PBR贴图
layout(binding = 1) uniform sampler2D u_normal_texture;
layout(binding = 2) uniform sampler2D u_albedo_texture;
layout(binding = 3) uniform sampler2D u_metallic_roughness_texture;

//toggle
uniform float u_blend_toggle;
uniform float u_normal_tex_toggle;
uniform float u_albedo_tex_toggle;
uniform float u_metallic_roughness_tex_toggle;

// IBL
uniform float u_ibl_exposure;
layout(binding = 11) uniform samplerCube u_irradiance_map;
layout(binding = 12) uniform samplerCube u_prefilter_map;
layout(binding = 13) uniform sampler2D u_brdf_LUT;

// CSM
uniform float u_cascade_plane_distances[4];
uniform int u_cascade_count;   // number of frusta - 1
uniform float u_csm_far_plane;
layout(binding = 14) uniform sampler2DArray u_CSM;

// OSM
uniform vec3 u_point_light_position;
uniform float u_osm_far_plane;
uniform vec3 u_point_light_color;
uniform float u_point_light_intensity;
layout(binding = 15) uniform samplerCube u_OSM;

const float PI = 3.14159265359;

// GGX分布
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float CSMShadowCalculation(vec3 fragPosWorldSpace)
{
    // select cascade layer
    vec4 fragPosViewSpace = camera.view * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for(int i = 0; i < u_cascade_count; ++i)
    {
        if(depthValue < u_cascade_plane_distances[i])
        {
            layer = i;
            break;
        }
    }
    if(layer == -1)
    {
        layer = u_cascade_count;
    }

    vec4 fragPosLightSpace = sun.light_space_matrices[layer] * vec4(fragPosWorldSpace, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(currentDepth > 1.0)
    {
        return 0.0;
    }
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = vec3(0.0, 0.0, 1.0);
    float bias = max(0.05 * (1.0 - dot(normal, -fs_in.tangent_sun_direction)), 0.005);
    const float biasModifier = 0.5;
    if(layer == u_cascade_count)
    {
        bias *= 1 / (u_csm_far_plane * biasModifier);
    }
    else
    {
        bias *= 1 / (u_cascade_plane_distances[layer] * biasModifier);
    }

    // bias = 0.0;

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(u_CSM, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_CSM, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // float pcfDepth = texture(u_CSM, vec3(projCoords.xy, layer)).r;
    // shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;

    return shadow;
}

vec3 gridSamplingDisk[20] = vec3[](vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1), vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1), vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0), vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1), vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1));

float OSMShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - u_point_light_position;
    // use the fragment to light vector to sample from the depth map    
    // float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    // closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    // float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    // float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    // PCF
    // float shadow = 0.0;
    // float bias = 0.05; 
    // float samples = 4.0;
    // float offset = 0.1;
    // for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    // {
        // for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        // {
            // for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            // {
                // float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
                // closestDepth *= far_plane;   // Undo mapping [0;1]
                // if(currentDepth - bias > closestDepth)
                    // shadow += 1.0;
            // }
        // }
    // }
    // shadow /= (samples * samples * samples);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(camera.position.xyz - fragPos);
    float diskRadius = (1.0 + (viewDistance / u_osm_far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(u_OSM, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= u_osm_far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    

    return shadow;
}

vec3 CalculateLo(vec3 light_direction, vec3 view_direction, vec3 normal, vec3 radiance, vec3 albedo, float metallic, float roughness)
{
    //半程向量
    vec3 H = normalize(view_direction + light_direction);

    //菲涅尔项的输入F0
    //对于绝缘体为0.04
    //对于金属根据反射率与金属度计算
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo.rgb, metallic);

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, H, roughness);
    float G = GeometrySmith(normal, view_direction, light_direction, roughness);
    vec3 F = fresnelSchlick(clamp(dot(H, view_direction), 0.0, 1.0), F0);

    // 镜面反射部分
    vec3 numerator = NDF * G * F;
    // 分母+0.0001防止除以0
    float denominator = 4.0 * max(dot(normal, view_direction), 0.0) * max(dot(normal, light_direction), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    // 镜面反射比率等于菲涅尔项
    vec3 Ks = F;
    // 漫反射比率等于1-镜面反射(能量守恒)
    vec3 Kd = vec3(1.0) - Ks;
    //金属度越高则漫反射越少
    Kd *= 1.0 - metallic;

    // N*L项
    float NdotL = max(dot(normal, light_direction), 0.0);

    //反射率方程
    //Ks这一项learnopengl说不用乘，后面再看看
    return (Kd * albedo.rgb / PI + Ks * specular) * radiance * NdotL;
}

void main()
{
    ////////////////////////////////////
    // 从纹理或是直接读入数据
    ////////////////////////////////////

    // 法向量
    vec3 tangent_normal = texture(u_normal_texture, fs_in.uv).rgb;
    tangent_normal = u_normal_tex_toggle > 0.5 ? normalize(tangent_normal * 2.0 - 1.0) : vec3(0.0, 0.0, 1.0);

    // 基础色 / 反照率 / 漫反射颜色
    vec4 albedo = (u_albedo_tex_toggle > 0.5) ? texture(u_albedo_texture, fs_in.uv) : u_albedo;

    // 金属度 & 粗糙度
    float metallic = u_metallic_roughness_tex_toggle > 0.5 ? texture(u_metallic_roughness_texture, fs_in.uv).b : u_metallic;
    float roughness = u_metallic_roughness_tex_toggle > 0.5 ? texture(u_metallic_roughness_texture, fs_in.uv).g : u_roughness;

    // if(u_blend_toggle > 0.5)
    // {
    //     metallic = 0.0f;
    //     roughness = 1.0f;
    // }

    ////////////////////////////////////
    // 光照计算
    ////////////////////////////////////

    //法线
    vec3 N = normalize(tangent_normal);
    //观察向量
    vec3 V = normalize(fs_in.tangent_camera_pos - fs_in.tangent_world_pos);
    //反射向量(光的入射方向)
    vec3 R = reflect(-V, N);

    //菲涅尔项的输入F0
    //对于绝缘体为0.04
    //对于金属根据反射率与金属度计算
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo.rgb, metallic);

    // 反射项
    vec3 Lo = vec3(0.0);
    // 自发光项
    vec3 Le = vec3(0.0);

    ////////////////////////////////////
    // 光源
    ////////////////////////////////////

    //直接光(太阳)
    //CSM计算阴影
    float shadow = CSMShadowCalculation(fs_in.world_pos);

    Lo += CalculateLo(-fs_in.tangent_sun_direction, V, N, sun.color.rgb * sun.intensity.r, albedo.xyz, metallic, roughness) * (1.0 - shadow);

    //点光源(灯笼)
    //目前只有一个
    //OSM
    shadow = OSMShadowCalculation(fs_in.world_pos);
    vec3 L = normalize(fs_in.tangent_point_light_position - fs_in.tangent_world_pos);
    //衰减
    float distance = length(u_point_light_position - fs_in.world_pos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = u_point_light_color * attenuation * u_point_light_intensity;

    Lo += CalculateLo(L, V, N, radiance, albedo.xyz, metallic, roughness) * (1.0 - shadow);

    ////////////////////////////////////
    // IBL
    ////////////////////////////////////

    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    //漫反射项
    //直接从irradiance map里采样
    vec3 irradiance = texture(u_irradiance_map, fs_in.TBN * N).rgb;
    vec3 diffuse = irradiance * albedo.rgb;

    //镜面项
    //预卷积和brdf_lut
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(u_prefilter_map, fs_in.TBN * R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(u_brdf_LUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    //LearnOpenGL说Ks不用乘 但是不懂
    vec3 ambient = kD * diffuse + kS * specular;

    //限制环境光亮度
    Lo += ambient * u_ibl_exposure;

    // 目前只有灯泡是自发光的
    Le = u_emissive * u_point_light_intensity;

    vec3 color = Lo + Le;

    ////////////////////////////////////
    // weighted blend
    ////////////////////////////////////

    // weight function
    float weight = clamp(pow(min(1.0, albedo.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

	// store pixel color accumulation
    accum = vec4(color * albedo.a, albedo.a) * weight;

	// store pixel revealage threshold
    reveal = albedo.a;

}