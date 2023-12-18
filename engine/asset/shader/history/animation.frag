#version 460 core
out vec4 FragColor;

in VS_OUT
{
    vec2 uv;
}
fs_in;

layout(binding = 1) uniform sampler2D u_diffuse_texture;

void main()
{
    FragColor = texture(u_diffuse_texture, fs_in.uv);
}
