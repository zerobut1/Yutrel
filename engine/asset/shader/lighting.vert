#version 450

layout(location = 0) out vec2 out_uv;

layout(push_constant) uniform constants
{
    float width;
    float height;
}
push_constants;

void main()
{
    out_uv      = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(out_uv * 2.0f - 1.0f, 0.0f, 1.0f);
    out_uv.x    = out_uv.x * push_constants.width / 3840.0;
    out_uv.y    = out_uv.y * push_constants.height / 2160.0;
}
