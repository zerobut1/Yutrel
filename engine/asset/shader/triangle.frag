#version 450

layout(location = 0)out vec4 outFragColor;

layout(location = 0)in vec3 inColor;

void main()
{
    //return red
    outFragColor = vec4(inColor, 1.0f);
}