#version 450

layout(location = 0) out vec4 out_frag_color;

layout(location = 0) in vec2 in_uv;

void main()
{
    out_frag_color = vec4(in_uv, 1.0, 1.0);
}