#version 460 core

layout(location = 0) out vec2 _uv;

void main()
{
    vec2 position = vec2(gl_VertexID % 2, gl_VertexID / 2) * 4.0 - 1;
    _uv = (position + 1) * 0.5;
    gl_Position = vec4(position, 0.0, 1.0);
}