#version 450
#extension GL_EXT_buffer_reference : require

layout(location = 0) out vec3 out_world_pos;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec4 out_tangent;
layout(location = 3) out vec2 out_uv;
layout(location = 4) out vec4 out_view_space_pos;

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
    mat4 model_matrix;
    VertexBuffer vertex_buffer;
}
push_constants;

#define SHADOWMAP_CASCADE_COUNT 4

layout(set = 0, binding = 0) uniform SceneData
{
    mat4 view;
    mat4 projection;
    vec3 camera_position;
    vec4 ambient_color;
    vec4 directional_light_color;
    vec3 directional_light_direction;
    float cascade_splits[SHADOWMAP_CASCADE_COUNT];
    mat4 directional_light_VP[SHADOWMAP_CASCADE_COUNT];
}
u_scene_data;

void main()
{
    Vertex v = push_constants.vertex_buffer.vertices[gl_VertexIndex];
    vec4 pos = push_constants.model_matrix * vec4(v.position, 1.0f);

    gl_Position = u_scene_data.projection * u_scene_data.view * pos;

    out_world_pos = pos.xyz;
    out_normal    = mat3(push_constants.model_matrix) * v.normal;
    out_tangent   = vec4(mat3(push_constants.model_matrix) * v.tangent.xyz, v.tangent.w);
    out_uv        = v.uv;

    out_view_space_pos = u_scene_data.view * pos;
}