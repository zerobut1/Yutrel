#version 450

layout(location = 0) out vec4 out_frag_color;

layout(location = 0) in vec3 in_uv;

layout(set = 0, binding = 1) uniform samplerCube u_skybox;

void main()
{
    vec3 env_color = texture(u_skybox, in_uv).rgb;

    env_color = env_color / (env_color + vec3(1.0));
    env_color = pow(env_color, vec3(1.0 / 2.2));

    // out_frag_color = vec4(env_color, 0.0);
    out_frag_color = vec4(0.2);
}