#version 460 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;
layout(location = 3) in vec3 a_bitangent;
layout(location = 4) in vec2 a_uv;

out VS_OUT
{
    vec2 uv;
    vec3 world_pos;
    vec3 tangent_world_pos;
    vec3 tangent_camera_pos;
    vec3 tangent_sun_direction;
    vec3 tangent_point_light_position;
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

layout(std140, binding = 1) uniform Sun
{
    vec4 color;
    vec4 direction;
    vec4 intensity;
    mat4 light_space_matrices[4];
}
sun;

uniform mat4 u_model;

// 点光源
// todo uniform
uniform vec3 u_point_light_position;

void main()
{

    vs_out.uv = a_uv;

    //TBN矩阵
    mat3 normal_matrix = transpose(inverse(mat3(u_model)));
    vec3 T = normalize(normal_matrix * a_tangent);
    vec3 N = normalize(normal_matrix * a_normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(T, N);
    mat3 TBN = mat3(T, B, N);
    vs_out.TBN = TBN;
    TBN = transpose(TBN);

    vec3 world_pos = vec3(u_model * vec4(a_position, 1.0));
    vec3 normal = normalize(transpose(inverse(mat3(u_model))) * a_normal);
    vs_out.world_pos = world_pos;
    vs_out.tangent_world_pos = TBN * world_pos;
    vs_out.tangent_camera_pos = TBN * camera.position.xyz;
    vs_out.tangent_sun_direction = TBN * sun.direction.xyz;
    vs_out.tangent_point_light_position = TBN * u_point_light_position;

    gl_Position = camera.projection * camera.view * u_model * vec4(a_position, 1.0);
}