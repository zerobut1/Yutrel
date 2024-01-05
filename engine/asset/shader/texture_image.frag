#version 450

layout(location = 0)in vec3 in_color;
layout(location = 1)in vec2 in_UV;

layout(location = 0)out vec4 out_frag_color;

layout(set = 0, binding = 0)uniform sampler2D u_display_texture;

void main()
{
    out_frag_color = texture(u_display_texture, in_UV);
}