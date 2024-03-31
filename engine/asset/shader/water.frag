#version 450

layout(location = 0) out vec4 out_frag_color;

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec2 in_uv;

layout(set = 0, binding = 1) uniform sampler2D u_normal_texture;

void main()
{
    vec3 normal = texture(u_normal_texture, in_position.xz).xyz * 2.0 - 1.0;

    out_frag_color = vec4(normal, 1.0);
}