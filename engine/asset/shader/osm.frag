#version 460 core
in vec4 FragPos;

uniform float u_far_plane;
uniform vec3 u_light_position;

void main()
{
    float lightDistance = length(FragPos.xyz - u_light_position);

    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / u_far_plane;

    // write this as modified depth
    gl_FragDepth = lightDistance;
}