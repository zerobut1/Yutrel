#version 450
#extension GL_EXT_buffer_reference : require

layout(location = 0)out vec3 outColor;
layout(location = 1)out vec2 outUV;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 uv;
};

layout(buffer_reference, std430)readonly buffer VertexBuffer {
    Vertex vertices[];
};

//push constants block
layout(push_constant)uniform constants
{
    mat4 model_matrix;
    VertexBuffer vertexBuffer;
} PushConstants;

layout(set = 1, binding = 0)uniform SceneData {
    mat4 view;
    mat4 proj;
    mat4 view_proj;
    vec4 ambient_color;
    vec4 sunlight_direction; // w for sun power
    vec4 sunlight_color;
} scene_data;

void main()
{
    //load vertex data from device adress
    Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];
    
    //output data
    gl_Position = scene_data.proj * scene_data.view * PushConstants.model_matrix * vec4(v.position, 1.0f);
    // gl_Position = PushConstants.model_matrix * vec4(v.position, 1.0f);
    outColor = v.normal.xyz;
    outUV = v.uv;
}