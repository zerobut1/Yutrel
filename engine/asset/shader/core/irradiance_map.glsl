#version 460 core

// compute diffuse irradiance map from an HDR environment cubemap texture

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
layout(binding = 0) uniform samplerCube environment_map;
layout(binding = 0, rgba16f) restrict writeonly uniform imageCube irradiance_map;

#define PI       3.141592653589793
#define PI2      6.283185307179586
#define INV_PI   0.318309886183791  // 1 over PI

vec3 Tangent2World(vec3 N, vec3 v) {
    N = normalize(N);

    // choose the up vector U that does not overlap with N
    vec3 U = mix(vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), step(abs(N.y), 0.999));
    vec3 T = normalize(cross(U, N));
    vec3 B = normalize(cross(N, T));
    return T * v.x + B * v.y + N * v.z;  // mat3(T, B, N) * v
}

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

vec3 UniformSampleHemisphere(float u, float v) {
    float phi = v * PI2;
    float cos_theta = 1.0 - u;  // ~ [0, 1]
    float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    return vec3(cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta);
}

vec3 CosineSampleHemisphere(float u, float v) {
    float phi = v * PI2;
    float cos_theta = sqrt(1.0 - u);  // bias toward cosine using the `sqrt` function
    float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    return vec3(cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta);
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


// this way of sampling over the hemisphere is simple but biased and slow.
vec3 NaiveConvolution(vec3 N, float h_step, float v_step) {
    vec3 irradiance = vec3(0.0);
    uint n_samples = 0;

    // tweak the step size based on the environment map resolution, 1K needs
    // a step size of ~ 0.025, 2K/4K requires 0.01 or even lower, very large
    // step size can lead to noticeable artifacts due to undersampling.
    // h_step: increment on the azimuth angle (longitude)
    // v_step: increment on the polar angle (latitude)

    for (float phi = 0.0; phi < PI2; phi += h_step) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += v_step) {
            vec3 L = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            L = Tangent2World(N, L);
            irradiance += texture(environment_map, L).rgb * cos(theta) * sin(theta);
            n_samples++;
        }
    }

    /* this expression already takes into account the Lambertian diffuse factor `INV_PI`
       o/w we should have multiplied by `PI squared` instead of `PI`, check the formula:
       http://www.codinglabs.net/article_physically_based_rendering.aspx

       if you still wonder why we multiply by `PI`, here's another perspective:
       every texture lookup is weighted by `sin(theta) * cos(theta) = sin(2 * theta) / 2`
       where the polar angle `theta` on the hemisphere ranges from 0 to `PI / 2`. Hence,
       the expected value of all weights is `1 / PI` if you solve the integral, that is,
       the PDF is 1 over PI, therefore we multiply `PI` back to compensate for that.
    */

    return PI * irradiance / float(n_samples);
}

// this way of sampling over the hemisphere is uniform, unbiased and faster.
// for 2K resolution we need ~ 16000 samples to avoid undersampling artifacts
vec3 UniformConvolution(vec3 N, uint n_samples) {
    vec3 irradiance = vec3(0.0);

    for (uint i = 0; i < n_samples; i++) {
        vec2 u = Hammersley2D(i, n_samples);
        vec3 L = UniformSampleHemisphere(u.x, u.y);
        L = Tangent2World(N, L);

        float NoL = max(dot(N, L), 0.0);
        irradiance += texture(environment_map, L).rgb * NoL;
    }

    // every texture lookup is weighted by `NoL` which ranges from 0 to 1,
    // and sampling is uniform over the hemisphere, so the averaged weight
    // is 0.5. To compensate for this, we need to double the result.

    return 2.0 * irradiance / float(n_samples);
}

// this way of sampling over the hemisphere is cosine-weighted, preciser and faster.
// for 2K resolution we only need ~ 8000 samples to achieve nice results.
vec3 CosineConvolution(vec3 N, uint n_samples) {
    vec3 irradiance = vec3(0.0);

    for (uint i = 0; i < n_samples; i++) {
        vec2 u = Hammersley2D(i, n_samples);
        vec3 L = CosineSampleHemisphere(u.x, u.y);
        L = Tangent2World(N, L);
        irradiance += texture(environment_map, L).rgb;
    }

    /* since the sampling is already cosine-weighted, we can directly sum up the retrieved texel
       values and divide by the total number of samples, there's no need to include a weight and
       then balance the result with a multiplier. If we multiply each texel by `NoL` and then
       double the result as we did in uniform sampling, we are essentially weighing the radiance
       twice, in which case the result irradiance map would be less blurred where bright pixels
       appear brighter and dark areas are darker, in fact many people were doing this wrong.
    */

    return irradiance / float(n_samples);
}

void main() {
    ivec3 ils_coordinate = ivec3(gl_GlobalInvocationID);
    vec2 resolution = vec2(imageSize(irradiance_map));
    vec3 N = ILS2Cartesian(ils_coordinate, resolution);

    // here we present 3 different ways of computing diffuse irradiance map from an HDR
    // environment map, all 3 have considered the cosine term in the integral, and will
    // yield results that are hardly distinguishable. The last one uses cosine-weighted
    // sampling, it's a lot more performant and requires much fewer samples to converge.

    // vec3 irradiance = NaiveConvolution(N, 0.01, 0.01);
    // vec3 irradiance = UniformConvolution(N, 16384);
    vec3 irradiance = CosineConvolution(N, 16384);

    imageStore(irradiance_map, ils_coordinate, vec4(irradiance, 1.0));
}