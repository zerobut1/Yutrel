#version 460 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;
layout(location = 3) in vec3 a_bitangent;
layout(location = 4) in vec2 a_uv;
//一般来说最多有四个骨骼与顶点有关，因此为四维向量
layout(location = 5) in ivec4 a_bone_ids;
layout(location = 6) in vec4 a_bone_weights;

out VS_OUT
{
    vec2 uv;
}
vs_out;

layout(std140, binding = 0) uniform Camera
{
    vec4 position;
    mat4 view;
    mat4 projection;
}
u_camera;
//模型矩阵
uniform mat4 u_model;

//骨骼
const int MAX_BONES = 150;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 u_bone_transform[MAX_BONES];

void main()
{
    vec4 totalPosition = vec4(0.0);
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if(a_bone_ids[i] == -1)
            continue;
        if(a_bone_ids[i] >= MAX_BONES)
        {
            totalPosition = vec4(a_position, 1.0);
            break;
        }
        vec4 localPosition = u_bone_transform[a_bone_ids[i]] * vec4(a_position, 1.0);
        totalPosition += localPosition * a_bone_weights[i];
        // vec3 localNormal = mat3(u_bone_transform[a_bone_ids[i]]) * norm;
    }

    mat4 viewModel = u_camera.view * u_model;
    gl_Position = u_camera.projection * viewModel * totalPosition;
    vs_out.uv = a_uv;
}