#version 450
#extension GL_EXT_buffer_reference : require

layout(location = 0) out vec3 out_uv;

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

layout(set = 0, binding = 0) uniform SceneData
{
    mat4 view;
    mat4 projection;
    vec3 camera_position;
    vec4 ambient_color;
    vec4 directional_light_color;
    vec3 directional_light_direction;
    float cascade_splits[4];
    mat4 directional_light_VP[4];
}
u_scene_data;

void main()
{
    Vertex v = push_constants.vertex_buffer.vertices[gl_VertexIndex];

    mat4 rot_view = mat4(mat3(u_scene_data.view));

    gl_Position = u_scene_data.projection * rot_view * vec4(v.position, 1.0f);

    out_uv = v.position;
}