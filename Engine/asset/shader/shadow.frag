#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform float u_DiffuseTexToggle;
uniform float u_SpecularTexToggle;
uniform float u_NormalsTexToggle;

uniform vec3 u_DiffuseColor;
uniform vec3 u_SpecularColor;

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
    vec3 normal = texture(u_TextureNormals, fs_in.TexCoords).rgb;
    normal = u_NormalsTexToggle > 0.5 ? normalize(normal * 2.0 - 1.0) : normalize(fs_in.Normal);
    // vec3 normal = normalize(fs_in.Normal);

    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
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

    vec3 normal = texture(u_TextureNormals, fs_in.TexCoords).rgb;
    normal = u_NormalsTexToggle > 0.5 ? normalize(normal * 2.0 - 1.0) : normalize(fs_in.Normal);
    // vec3 normal = normalize(fs_in.Normal);

    vec3 lightColor = vec3(1.0);

    // ambient & diffuse
    vec3 material_diffuse = u_DiffuseTexToggle > 0.5 ? texture(u_TextureDiffuse, fs_in.TexCoords).rgb : u_DiffuseColor;
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 ambient = material_diffuse * lightColor;
    vec3 diffuse = diff * material_diffuse * lightColor;

    // specular
    vec3 material_specular = u_SpecularTexToggle > 0.5 ? texture(u_TextureSpecular, fs_in.TexCoords).rgb : u_SpecularColor;
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * material_specular * lightColor;

    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);

    FragColor = vec4(lighting, 1.0);
    // FragColor = vec4(material_ambient, 1.0);
    // FragColor = vec4(material_diffuse, 1.0);
    // FragColor = vec4(ambient + diffuse + specular, 1.0);
}