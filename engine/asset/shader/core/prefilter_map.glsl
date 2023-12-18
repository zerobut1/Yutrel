#version 460 core

// compute specular prefiltered environment map

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
layout(binding = 0) uniform samplerCube environment_map;
layout(binding = 1, rgba16f) restrict writeonly uniform imageCube prefilter_map;

layout(location = 0) uniform float roughness;

/* this part of the integral does not have the cosine term, we simply average all the
   incoming radiance Li over the hemisphere. Using GGX importance sampling, we narrow
   down the hemisphere to a smaller region (the specular lobe) defined by the GGX NDF
   so that most samples will be drawn from within this lobe, this effectively reduces
   the total number of samples needed for convergence.

   the shape of the GGX NDF depends on the view direction as well, to eliminate this
   dimension, the "split sum" approximation further assumes that the viewing angle is
   zero, i.e. `N = V = R`. Note that this only works with isotropic reflection models.
   for every sample we draw, the retrieved texel color is weighted by `NoL`, doing so
   can reduce the error from the `N = V = R` assumption (as suggested by Epic Games),
   the idea is as follows:

   the more the surface normal `N` projects onto the incident light direction `L`, the
   more that texel should contribute to the prefiltered specular color. If `NoL = 0`,
   `L` is perpendicular to `N`, so it's nearly parallel to the surface, as if we are
   viewing from the grazing angle. Conversely, if `NoL = 1`, `N` and `L` overlaps and
   we are viewing at normal incidence, thus the specular component must be highest.

   for environment maps that include high frequency details, such convolution can lead
   to aliasing effects due to undersampling, especially when the resolution is >= 2K.
   to reduce the artifact, NVIDIA presented a technique called "filtering with mipmaps"
   which is able to produce smoother, more visually acceptable results w/o increasing
   the sample size. Reference:

   https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch20.html Section 20.4
   https://github.com/Nadrin/PBR/blob/master/data/shaders/glsl/spmap_cs.glsl

   the idea of filtering with mipmaps is as follows:
   the lower the PDF, the more pixels from the environment should be averaged for that
   sample, this roughly translates to using lower mipmap level. On the other hand, if
   the PDF is very high, many samples are likely to be drawn in similar directions, so
   they can help average out the error in the integral estimation from this region. In
   this case, the sample should only average a small area of the environment map, so a
   higher mipmap level should be used.
*/

#define PI       3.141592653589793
#define PI2      6.283185307179586
#define INV_PI   0.318309886183791  // 1 over PI

float RadicalInverse_VdC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;  // (1 / 0x100000000)
}

vec2 Hammersley2D(uint i, uint N) {
    return vec2(float(i) / float(N), RadicalInverse_VdC(i));
}

vec3 Tangent2World(vec3 N, vec3 v) {
    N = normalize(N);

    // choose the up vector U that does not overlap with N
    vec3 U = mix(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), step(abs(N.y), 0.999));
    vec3 T = normalize(cross(U, N));
    vec3 B = normalize(cross(N, T));
    return T * v.x + B * v.y + N * v.z;  // mat3(T, B, N) * v
}

vec3 ImportanceSampleGGX(float u, float v, float alpha) {
    float a2 = alpha * alpha;
    float phi = u * PI2;
    float cos_theta = sqrt((1.0 - v) / (1.0 + (a2 - 1.0) * v));  // bias toward cosine and TRGGX NDF
    float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    return vec3(cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta);
}

float D_TRGGX(float alpha, float NoH) {
    float a = NoH * alpha;
    float k = alpha / (1.0 - NoH * NoH + a * a);
    return k * k * INV_PI;
}

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

vec3 ILS2Cartesian(ivec3 w, vec2 resolution) {
    // w often comes from a compute shader in the form of `gl_GlobalInvocationID`
    vec2 st = w.xy / resolution;  // tex coordinates in [0, 1] range
    return UV2Cartesian(st, w.z);
}


vec3 PrefilterEnvironmentMap(vec3 R, uint n_samples) {
    vec3 N = R;
    vec3 V = R;

    vec2 env_size = vec2(textureSize(environment_map, 0));
    float w = 4.0 * PI / (6 * env_size.x * env_size.y);  // solid angle per texel (Equation 12)

    // roughness is guaranteed to be > 0 as the base level is copied directly
    float alpha = roughness * roughness;

    float weight = 0.0;
    vec3 color = vec3(0.0);

    for (uint i = 0; i < n_samples; i++) {
        vec2 u = Hammersley2D(i, n_samples);
        vec3 H = Tangent2World(N, ImportanceSampleGGX(u.x, u.y, alpha));
        vec3 L = 2 * dot(H, V) * H - V;

        float NoH = max(dot(N, H), 0.0);
        float NoL = max(dot(N, L), 0.0);
        float HoV = max(dot(H, V), 0.0);

        if (NoL > 0.0) {
            float pdf = D_TRGGX(NoH, alpha) * NoH / (4.0 * HoV);
            float ws = 1.0 / (n_samples * pdf + 0.0001);  // solid angle associated with this sample (Equation 11)
            float mip_level = max(0.5 * log2(ws / w) + 1.0, 0.0);  // mipmap level (Equation 13 biased by +1)
            color += textureLod(environment_map, L, mip_level).rgb * NoL;
            weight += NoL;
        }
    }

    return color / weight;
}

void main() {
    ivec3 ils_coordinate = ivec3(gl_GlobalInvocationID);
    vec2 resolution = vec2(imageSize(prefilter_map));

    // make sure we won't write past the texture when computing higher mipmap levels
    if (ils_coordinate.x >= resolution.x || ils_coordinate.y >= resolution.y) {
        return;
    }

    vec3 R = ILS2Cartesian(ils_coordinate, resolution);
    vec3 color = PrefilterEnvironmentMap(R, 2048);  // ~ 1024 samples for 2K resolution

    imageStore(prefilter_map, ils_coordinate, vec4(color, 1.0));
}
