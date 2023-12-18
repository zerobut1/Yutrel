#version 460 core

// convert a (2:1) equirectangle texture to a (1:1 x 6) cubemap texture

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
layout(binding = 0) uniform sampler2D equirectangle;
layout(binding = 0, rgba16f) restrict writeonly uniform imageCube cubemap;

#define PI       3.141592653589793
#define PI2      6.283185307179586

// convert a 2D texture coordinate st on a cubemap face to its equivalent 3D
// texture lookup vector v such that `texture(cubemap, v) == texture(face, st)`
vec3 UV2Cartesian(vec2 st, uint face) {
    vec3 v = vec3(0.0);  // texture lookup vector in world space
    vec2 uv = 2.0 * vec2(st.x, 1.0 - st.y) - 1.0;  // convert [0, 1] to [-1, 1] and invert y

    // https://en.wikipedia.org/wiki/Cube_mapping#Memory_addressing
    switch(face) {
        case 0:
            v = vec3(+1.0, uv.y, -uv.x);
            break;  // posx
        case 1:
            v = vec3(-1.0, uv.y, uv.x);
            break;  // negx
        case 2:
            v = vec3(uv.x, +1.0, -uv.y);
            break;  // posy
        case 3:
            v = vec3(uv.x, -1.0, uv.y);
            break;  // negy
        case 4:
            v = vec3(uv.x, uv.y, +1.0);
            break;  // posz
        case 5:
            v = vec3(-uv.x, uv.y, -1.0);
            break;  // negz
    }

    return normalize(v);
}

// convert an ILS image coordinate w to its equivalent 3D texture lookup
// vector v such that `texture(samplerCube, v) == imageLoad(imageCube, w)`
vec3 ILS2Cartesian(ivec3 w, vec2 resolution) {
    // w often comes from a compute shader in the form of `gl_GlobalInvocationID`
    vec2 st = w.xy / resolution;  // tex coordinates in [0, 1] range
    return UV2Cartesian(st, w.z);
}

// convert a vector v in Cartesian coordinates to spherical coordinates
vec2 Cartesian2Spherical(vec3 v) {
    float phi = atan(v.z, v.x);          // ~ [-PI, PI] (assume v is normalized)
    float theta = acos(v.y);             // ~ [0, PI]
    return vec2(phi / PI2, theta / PI);  // ~ [-0.5, 0.5], [0, 1]
}

// remap a spherical vector v into equirectangle texture coordinates
vec2 Spherical2Equirect(vec2 v) {
    return vec2(v.x + 0.5, v.y);  // ~ [0, 1]
}

void main() {
    vec2 resolution = vec2(imageSize(cubemap));
    ivec3 ils_coordinate = ivec3(gl_GlobalInvocationID);

    vec3 v = ILS2Cartesian(ils_coordinate, resolution);

    vec2 sample_vec = Cartesian2Spherical(v);
    sample_vec = Spherical2Equirect(sample_vec);
    vec4 color = texture(equirectangle, sample_vec);

    imageStore(cubemap, ils_coordinate, color);
}
