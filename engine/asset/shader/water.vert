#version 450
#extension GL_EXT_buffer_reference : require

layout(location = 0) out vec2 out_uv;

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
}
push_constants;

layout(set = 0, binding = 0) uniform SceneData
{
    mat4 view;
    mat4 projection;
}
u_scene_data;

void main()
{
    Vertex v = push_constants.vertex_buffer.vertices[gl_VertexIndex];

    gl_Position = u_scene_data.projection * u_scene_data.view * push_constants.model * vec4(v.position, 1.0f);

    out_uv = v.uv;
}