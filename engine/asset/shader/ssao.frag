#version 460 core
layout(location = 0) out float ssao;
// out vec3 ssao;

in vec2 _uv;

layout(std140, binding = 0) uniform Camera
{
    vec4 position;
    mat4 view;
    mat4 projection;
}
camera;

// g-buffer
layout(binding = 20) uniform sampler2D g_positon_depth;
layout(binding = 21) uniform sampler2D g_normal;

// 切线空间的采样向量
uniform vec3 u_samples[64];

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

//简单的随机数函数
float random(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    // get input for SSAO algorithm
    vec3 world_position = texture(g_positon_depth, _uv).xyz;
    vec3 viewspace_position = vec3(camera.view * vec4(world_position, 1.0));
    vec3 world_normal = normalize(texture(g_normal, _uv).rgb);

    //获取一个随机向量,用以将采样空间略微旋转
    vec3 randomVec = normalize(vec3(random(_uv), random(_uv + 0.5), random(_uv + 1)));

    // TBN矩阵把切线空间的采样点转换到世界空间
    vec3 tangent = normalize(randomVec - world_normal * dot(randomVec, world_normal));
    vec3 bitangent = cross(world_normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, world_normal);

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // 得到世界空间中的采样点
        vec3 samplePos = TBN * u_samples[i];
        samplePos = world_position + samplePos * radius; 

        // 采样点对应的纹理采样uv
        vec4 offset = vec4(samplePos, 1.0);
        // 变换到裁剪空间
        offset = camera.projection * camera.view * offset;
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        // 得到采样点对应gbuffer的深度值
        float sampleDepth = -texture(g_positon_depth, offset.xy).w;

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(viewspace_position.z - sampleDepth));
        occlusion += (sampleDepth >= viewspace_position.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);

    ssao = occlusion;
}