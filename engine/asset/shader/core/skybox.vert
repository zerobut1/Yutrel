#version 460 core
layout(location = 0) in vec3 aPos;

out vec3 localPos;

layout(std140, binding = 0) uniform Camera
{
    vec4 u_camera_pos;
    mat4 u_view;
    mat4 u_projection;
};

void main()
{
    localPos = aPos;

    mat4 rotView = mat4(mat3(u_view)); // remove translation from the view matrix
    vec4 clipPos = u_projection * rotView * vec4(localPos, 1.0);

    gl_Position = clipPos.xyww;
}