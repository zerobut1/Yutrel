#version 450
#extension GL_EXT_buffer_reference : require

layout(location = 0) out vec4 out_position;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out vec2 out_screen_uv;
layout(location = 3) out vec3 out_normal;
layout(location = 4) out vec4 out_tangent;
layout(location = 5) out float out_time;

struct Vertex
{
    vec3 position;
    vec3 normal;
    vec4 tangent;
    vec2 uv;
};

layout(buffer_reference, std430) readonly buffer VertexBuffer
{
    Vertex vertices[];
};

layout(push_constant) uniform constants
{
    mat4 model;
    VertexBuffer vertex_buffer;
    float width;
    float height;
    float time;
}
push_constants;

layout(set = 0, binding = 0) uniform SceneData
{
    mat4 view;
    mat4 projection;
}
u_scene_data;

#define PI 3.1415926535897932384626433832795

// vec3 GerstnerWave(vec4 wave, vec3 p, inout vec3 tangent, inout vec3 binormal)
// {
//     float steepness  = wave.z;
//     float wavelength = wave.w;
//     float k          = 2 * PI / wavelength;
//     float c          = 0.001 * sqrt(9.8 / k); // _WaveSpeed
//     vec2 d           = normalize(wave.xy);
//     float f          = k * (dot(d, p.xz) - c * push_constants.time);
//     float a          = steepness / k; // _Amplitude

//     tangent += vec3(
//         -d.x * d.x * (steepness * sin(f)),
//         d.x * (steepness * cos(f)),
//         -d.x * d.y * (steepness * sin(f)));
//     binormal += vec3(
//         -d.x * d.y * (steepness * sin(f)),
//         d.y * (steepness * cos(f)),
//         -d.y * d.y * (steepness * sin(f)));
//     return vec3( // 输出顶点偏移量
//         d.x * (a * cos(f)),
//         a * sin(f),
//         d.y * (a * cos(f)));
// }

vec3 GerstnerWave(vec4 wave, vec3 p, inout vec3 tangent, inout vec3 binormal)
{
    vec2 d           = wave.xy;
    float steepness  = wave.z;
    float wavelength = wave.w;

    float k          = 2 * PI / wavelength;
    float w          = 0.0001 * sqrt(9.81 / k); // _WaveSpeed
    float a          = steepness / k;           // _Amplitude
    vec2 wave_vector = k * normalize(wave.xy);
    float value      = (dot(wave_vector, p.xz) - w * push_constants.time);

    tangent += vec3(
        -d.x * d.x * steepness * sin(value),
        d.x * steepness * cos(value),
        -d.x * d.y * steepness * sin(value));
    binormal += vec3(
        -d.x * d.y * steepness * sin(value),
        d.y * steepness * cos(value),
        -d.y * d.y * steepness * sin(value));
    return vec3( // 输出顶点偏移量
        d.x * (a * cos(value)),
        a * sin(value),
        d.y * (a * cos(value)));
}

void main()
{
    Vertex v = push_constants.vertex_buffer.vertices[gl_VertexIndex];

    vec4 pos = push_constants.model * vec4(v.position, 1.0f);

    // float l     = 20.0;
    // float w     = 0.001;
    // float a     = 1.0;
    // float k     = 2 * PI / l;
    // float value = k * pos.x - w * push_constants.time;
    // // float value = k * pos.x;
    // pos.x += a * cos(value);
    // pos.y += a * sin(value);

    vec3 gridPoint = pos.xyz;
    vec3 tangent   = vec3(1.0, 0.0, 0.0);
    vec3 binormal  = vec3(0.0, 0.0, 1.0);
    vec3 p         = gridPoint;
    // 注意两个wave都是针对于原顶点gridPoint的偏移
    // xy direction
    // z stepness
    // w wavelength
    vec4 wave_a = vec4(0.4, 0.7, 0.3, 12);
    vec4 wave_b = vec4(0.5, -1.0, 0.7, 20);
    vec4 wave_c = vec4(0.1, -0.4, 0.45, 15);

    // float sum_stepness = wave_a.z + wave_b.z;
    // wave_a.z /= sum_stepness;
    // wave_b.z /= sum_stepness;

    p += GerstnerWave(wave_a, gridPoint, tangent, binormal);
    p += GerstnerWave(wave_b, gridPoint, tangent, binormal);
    p += GerstnerWave(wave_c, gridPoint, tangent, binormal);
    // pos.xyz         = p;
    out_normal      = normalize(cross(binormal, tangent));
    out_tangent.xyz = tangent;

    out_position    = pos;
    vec4 _pos       = u_scene_data.projection * u_scene_data.view * out_position;
    gl_Position     = _pos;
    out_uv          = v.uv;
    out_screen_uv   = ((_pos.xy / _pos.w) + 1.0) / 2.0;
    out_screen_uv.x = out_screen_uv.x * push_constants.width / 3840.0;
    out_screen_uv.y = out_screen_uv.y * push_constants.height / 2160.0;

    out_time = push_constants.time;

    // vec3 binormal = vec3(0.0, 0.0, 1.0);
    // vec3 tangent  = vec3(1.0 - a * k * sin(value), a * k * cos(value), 0.0);
    // vec3 normal   = normalize(cross(binormal, tangent));
}