#version 460 core

layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;

layout(std140, binding = 1) uniform Sun
{
    vec4 color;
    vec4 direction;
    vec4 intensity;
    mat4 light_space_matrices[4];
}
sun;

void main()
{          
	for (int i = 0; i < 3; ++i)
	{
		gl_Position = sun.light_space_matrices[gl_InvocationID] * gl_in[i].gl_Position;
		gl_Layer = gl_InvocationID;
		EmitVertex();
	}
	EndPrimitive();
}  
