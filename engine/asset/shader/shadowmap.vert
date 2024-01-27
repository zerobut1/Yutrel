#version 450
#extension GL_EXT_buffer_reference : require

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
    mat4 light_MVP;
    VertexBuffer vertex_buffer;
} push_constants;

void main()
{
    Vertex v = push_constants.vertex_buffer.vertices[gl_VertexIndex];
    
    gl_Position = push_constants.light_MVP * vec4(v.position, 1.0f);
}