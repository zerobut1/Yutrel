#version 450

layout(location = 0)in vec2 in_uv;

layout(location = 0)out vec4 out_frag_color;

layout(set = 0, binding = 0)uniform sampler2D u_image;

float LinearizeDepth(float z)
{
    float n = 0.1f;
    float f = 1000.0f;
    return (n * (1.0f - z)) / ((f - n) * z+n);
}

void main()
{
    float z = texture(u_image, in_uv).r;
    out_frag_color = vec4(vec3(1.0 - LinearizeDepth(z)), 1.0);
}