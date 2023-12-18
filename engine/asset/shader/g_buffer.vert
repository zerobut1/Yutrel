#version 460 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;
layout(location = 3) in vec3 a_bitangent;
layout(location = 4) in vec2 a_uv;

out VS_OUT
{
    vec2 uv;
    vec3 world_position;
    vec3 world_normal;
    mat3 TBN;
}
vs_out;

layout(std140, binding = 0) uniform Camera
{
    vec4 position;
    mat4 view;
    mat4 projection;
}
camera;

uniform mat4 u_model;

void main()
{
    vs_out.uv = a_uv;

    vec3 world_pos = vec3(u_model * vec4(a_position, 1.0));
    vec3 normal = normalize(transpose(inverse(mat3(u_model))) * a_normal);
    vs_out.world_position = world_pos;
    vs_out.world_normal = normal;

    //TBN矩阵
    mat3 normal_matrix = transpose(inverse(mat3(u_model)));
    vec3 T = normalize(normal_matrix * a_tangent);
    vec3 N = normalize(normal_matrix * a_normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(T, N);
    mat3 TBN = mat3(T, B, N);
    vs_out.TBN = TBN;

    gl_Position = camera.projection * camera.view * u_model * vec4(a_position, 1.0);
}