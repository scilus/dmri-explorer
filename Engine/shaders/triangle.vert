#version 460

// per vertex attributes
layout (location=0) in uvec2 in_neighboursID;
layout (location=1) in vec3 in_position;

// Uniform buffer objects
layout(std430, binding = 2) buffer instanceTransformsBuffer
{
    mat4 modelMatrix[];
};

layout(std430, binding = 3) buffer cameraBuffer
{
    vec4 eye;
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

layout(std430, binding = 4) buffer sphHarmCoeffsBuffer
{
    float shCoeffs[]; // 45 consecutive values per voxel
};

layout(std430, binding = 5) buffer sphHarmFuncsBuffer
{
    float shFuncs[]; // 45 consecutive values per vertice
};

layout(std430, binding = 6) buffer nbVerticesBuffer
{
    uint nbVertices;
};

// Outputs
out gl_PerVertex{
    vec4 gl_Position;
};
out vec3 v_color;
out vec3 v_normal;
out vec4 v_eye;

// Constants
const int NB_SH = 45;

int jj(int l, int m)
{
    return l * (l + 1) / 2 + m;
}

float evaluateSH(uint sphVertID)
{
    float ret = 0.0f;
    float sum = 0.0f;
    for(int l = 0; l <= 8; l+=2)
    {
        for(int m = -l; m <= l; ++m)
        {
            sum += abs(shCoeffs[gl_DrawID * NB_SH + jj(l, m)]);
            ret += shCoeffs[gl_DrawID * NB_SH + jj(l, m)]
                 * shFuncs[sphVertID * NB_SH + jj(l, m)];
        }
    }
    if(sum > -1e-10)
    {
        return ret / sum;
    }
    return 0.0;
}

vec3 toCartesianCoordinates(float r, float theta, float phi)
{
    vec3 dir;
    dir.y = r * sin(phi) * sin(theta);
    dir.z = r * cos(theta);
    dir.x = r * cos(phi) * sin(theta);
    return dir;
}

vec3 toSF(float theta, float phi, uint sphVertID)
{
    float r = evaluateSH(sphVertID);
    return toCartesianCoordinates(r, theta, phi);
}

void main()
{
    float in_theta = in_position.y;
    float in_phi = in_position.z;
    uint sphVertID = gl_VertexID % nbVertices;

    vec3 position = toSF(in_theta, in_phi, sphVertID);
    gl_Position = projectionMatrix
                * viewMatrix
                * modelMatrix[gl_DrawID]
                * vec4(position, 1.0);

    v_normal = normalize(position);
    v_color = abs(v_normal);
    v_eye = normalize(eye);
}
