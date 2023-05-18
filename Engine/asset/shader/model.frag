#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoords);
    // FragColor = vec4(0.4, 0.8, 1.0,1.0);
    // FragColor = vec4(TexCoords, 0.0, 1.0);
}