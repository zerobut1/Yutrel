#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float u_AmbientTexToggle;
uniform float u_DiffuseTexToggle;
uniform float u_SpecularTexToggle;
uniform float u_NormalsTexToggle;

uniform vec3 u_AmbientColor;
uniform vec3 u_DiffuseColor;
uniform vec3 u_SpecularColor;
uniform vec3 u_NormalsColor;

layout(binding = 0) uniform sampler2D u_TextureAmbient;
layout(binding = 1) uniform sampler2D u_TextureDiffuse;
layout(binding = 2) uniform sampler2D u_TextureSpecular;
layout(binding = 3) uniform sampler2D u_TextureNormals;
layout(binding = 4) uniform sampler2D u_TextureShadowmap;

float ShadowCalculation(vec4 fragPosLightSpace) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_TextureShadowmap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
// Calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_TextureShadowmap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(u_TextureShadowmap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}

void main() {
    vec3 color = u_DiffuseTexToggle > 0.5 ? texture(u_TextureDiffuse, fs_in.TexCoords).rgb : u_DiffuseColor;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);
    // ambient
    vec3 ambient = 0.3 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);
}