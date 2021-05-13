#version 460

layout (location=0) in vec3 in_pos;
layout (location=1) in vec3 in_color;
layout (location=6) in vec3 in_normal;
layout (location=8) in float in_theta;
layout (location=9) in float in_phi;

// SSBO
layout(std430, binding = 3) buffer instanceTransforms
{
    mat4 modelMatrix[];
};

layout(std430, binding = 4) buffer camera
{
    vec4 eye;
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

layout(std430, binding = 7) buffer sphHarms
{
    float shCoeffs[];
};

// Outputs
out gl_PerVertex{
    vec4 gl_Position;
};
out vec3 v_color;
out vec3 v_normal;
out vec4 v_eye;

const float PI = 3.14159265358f;
const int MAX_FACTORIAL = 18;
const float FACTORIALS_LUT[MAX_FACTORIAL + 1] =
{
    1.0f,
    1.0f,
    2.0f,
    6.0f,
    24.0f,
    120.0f,
    720.0f,
    5040.0f,
    40320.0f,
    362880.0f,
    3628800.0f,
    39916800.0f,
    479001600.0f,
    6227020800.0f,
    87178291200.0f,
    1307674368000.0f,
    20922789888000.0f,
    355687428096000.0f,
    6402373705728000.0f
};

float factorial(int n)
{
    return FACTORIALS_LUT[n];
}

float combination(int n, int k)
{
    return factorial(n) / factorial(k) / factorial(n - k);
}

float assoc_legendre(int l, int m, float x)
{
    if(l == 0 && m == 0)
    {
        return 1.0f;
    }
    else if(l == 2 && m == -2)
    {
        return 1.0f / 24.0f * 3.0f * (1 - x*x);
    }
    else if(l == 2 && m == -1)
    {
        return -1.0f / 6.0f * -3.0f * x * pow(1 - x*x, 0.5f);
    }
    else if(l == 2 && m == 0)
    {
        return 0.5f * (3 * x*x - 1.0f);
    }
    else if(l == 2 && m == 1)
    {
        return -3.0f * x * pow(1 - x*x, 0.5f);
    }
    else if(l == 2 && m == 2)
    {
        return 3.0f * (1 - x*x);
    }
    return 0.0f;
}

float sph_harmonic_complex_radius(int l, int m, float theta)
{
    float r = (2.0f * l + 1) / 4.0f / PI
            * factorial(l - m) / factorial(l + m);

    r = sqrt(r) * assoc_legendre(l, m, cos(theta));
    return r;
}

float sph_harmonic_r(int l, int m, float theta, float phi)
{
    float r = sph_harmonic_complex_radius(l, m, theta);
    return r * cos(m * phi);
}

float sph_harmonic_i(int l, int m, float theta, float phi)
{
    float r = sph_harmonic_complex_radius(l, m, theta);
    return r * sin(m * phi);
}

float real_sh_descoteaux(int l, int m, float theta, float phi)
{
    if(m < 0)
    {
        return sqrt(2) * sph_harmonic_r(l, abs(m), theta, phi);
    }
    else if(m > 0)
    {
        return sqrt(2) * sph_harmonic_i(l, m, theta, phi);
    }
    else
    {
        return sph_harmonic_r(l, m, theta, phi);
    }
}

float evaluateSH(float theta, float phi)
{
    float ret = 0.0f;
    int offset = 0;
    for(int l = 0; l <= 8; l+=2)
    {
        for(int m = -l; m <= l; ++m)
        {
            ret += shCoeffs[gl_DrawID*45 + offset] * real_sh_descoteaux(l, m, theta, phi);
            ++offset;
        }
    }
    return ret;
}

vec3 toCartesianCoordinates(float r, float theta, float phi)
{
    vec3 dir;
    dir.y = r * sin(phi) * sin(theta);
    dir.z = r * cos(theta);
    dir.x = r * cos(phi) * sin(theta);
    return dir;
}

vec3 toSF(float theta, float phi)
{
    float r = evaluateSH(theta, phi);
    return toCartesianCoordinates(r, theta, phi);
}

vec3 calcNormal(vec3 pos, float theta, float phi)
{
    vec3 v1 = toSF(theta + 0.1, phi);
    vec3 v2 = toSF(theta, phi + 0.1);
    vec3 n = cross(pos - v1, pos - v2);
    return normalize(n);
}

void main()
{
    vec3 position = toSF(in_theta, in_phi);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix[gl_DrawID] * vec4(position, 1.0);

    v_color = abs(toCartesianCoordinates(1.0f, in_theta, in_phi));
    v_normal = calcNormal(position, in_theta, in_phi);
    v_eye = normalize(eye);
}
