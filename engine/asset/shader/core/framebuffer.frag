#version 460 core

layout(location = 0) in vec2 _uv;
layout(location = 0) out vec4 color;

layout(binding = 0) uniform sampler2D color_depth_texture;
layout(binding = 1) uniform usampler2D stencil_texture;

const float near = 0.1;
const float far = 100.0;

// typedef the subroutine (like a function pointer)
subroutine vec4 DrawBuffer(void);
layout(location = 0) subroutine uniform DrawBuffer drawbuffer;

layout(index = 0) subroutine(DrawBuffer) vec4 DrawColorBuffer()
{
    return texture(color_depth_texture, _uv);
}
// depth in screen space is non-linear, its precision is high for small z-values and low for
// large z-values so most pixels are white, we must linearize the depth value before drawing
layout(index = 1) subroutine(DrawBuffer) vec4 DrawDepthBuffer()
{
    float depth = texture(color_depth_texture, _uv).r;  // ~ [0,1] but non-linear
    float ndc_depth = depth * 2.0 - 1.0;  // ~ [-1,1]
    float z = (2.0 * near * far) / (far + near - ndc_depth * (far - near));  // ~ [near, far]
    float linear_depth = z / far;  // ~ [0,1]
    return vec4(vec3(linear_depth), 1.0);
}

layout(index = 2) subroutine(DrawBuffer) vec4 DrawStencilBuffer()
{
    uint stencil = texture(stencil_texture, _uv).r;
    return vec4(vec3(stencil), 1.0);
}

void main()
{
    color = vec4(drawbuffer().rgb, 1.0);
    // color = vec4(drawbuffer().rrr, 1.0);
    // color = drawbuffer();
}