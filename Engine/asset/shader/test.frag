#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture;

void main()
{
    // FragColor = vec4(0.4, 0.8, 1.0, 1.0);
    // FragColor = vec4(TexCoord,0.0,1.0);
    FragColor = texture2D(texture, TexCoord);
}