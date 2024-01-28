#version 450
#extension GL_EXT_buffer_reference : require

layout(location = 0)out vec2 out_uv;
layout(location = 1)out vec3 out_normal;
layout(location = 2)out vec4 out_tangent;
layout(location = 3)out vec3 out_view_vec;
layout(location = 4)out vec4 out_directional_light_shadow_coord;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec4 tangent;
    vec2 uv;
};

layout(buffer_reference, std430)readonly buffer VertexBuffer {
    Vertex vertices[];
};

layout(push_constant)uniform constants
{
    mat4 directional_light_VP;
    mat4 model_matrix;
    VertexBuffer vertex_buffer;
} push_constants;

layout(set = 1, binding = 0)uniform SceneData {
    mat4 view;
    mat4 proj;
    mat4 view_proj;
    vec4 view_position;
    vec4 ambient_color;
    vec4 sunlight_direction;
    vec4 sunlight_color;
} u_scene_data;

const mat4 bias_mat = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
0.5, 0.5, 0.0, 1.0);

void main()
{
    
    Vertex v = push_constants.vertex_buffer.vertices[gl_VertexIndex];
    vec4 pos = push_constants.model_matrix * vec4(v.position, 1.0f);
    
    gl_Position = u_scene_data.proj * u_scene_data.view * pos;
    
    out_uv = v.uv;
    out_normal = mat3(push_constants.model_matrix) * v.normal;
    out_tangent = v.tangent;
    out_view_vec = u_scene_data.view_position.xyz - pos.xyz;
    out_directional_light_shadow_coord = bias_mat * push_constants.directional_light_VP * pos;
}