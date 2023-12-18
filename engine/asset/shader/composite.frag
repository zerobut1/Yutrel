#version 460 core
out vec4 frag_color;

// layout(location = 0) in vec2 _uv;

// color accumulation buffer
layout(binding = 0) uniform sampler2D accum;

// revealage threshold buffer
layout(binding = 1) uniform sampler2D reveal;

// epsilon number
const float EPSILON = 0.00001;

// calculate floating point numbers equality accurately
bool isApproximatelyEqual(float a, float b)
{
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

// get the max value between three values
float max3(vec3 v)
{
    return max(max(v.x, v.y), v.z);
}

void main()
{
	// fragment coordination
    ivec2 coords = ivec2(gl_FragCoord.xy);

	// fragment revealage
    float revealage = texelFetch(reveal, coords, 0).r;

	// save the blending and color texture fetch cost if there is not a transparent fragment
    if(isApproximatelyEqual(revealage, 1.0))
        discard;

	// fragment color
    vec4 accumulation = texelFetch(accum, coords, 0);

	// suppress overflow
    if(isinf(max3(abs(accumulation.rgb))))
        accumulation.rgb = vec3(accumulation.a);

	// prevent floating point precision bug
    vec3 color = accumulation.rgb / max(accumulation.a, EPSILON);

    ////////////////////////////////////
    // 伽马矫正
    ////////////////////////////////////

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0 / 2.2));

	// blend pixels
    frag_color = vec4(color, 1.0 - revealage);
}