#version 460

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

layout(std430, binding = 6) buffer sphereInfoBuffer
{
    uint nbVertices;
    uint nbIndices;
};

layout(std430, binding = 7) buffer verticesBuffer
{
    float vertices[]; // {vx, vy, vz, vx, vy, ...}
};

layout(std430, binding = 8) buffer normalsBuffer
{
    float normals[]; // {nx, ny, nz, nx, ny, ...}
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

int flatten(int l, int m)
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
            sum += abs(shCoeffs[gl_DrawID * NB_SH + flatten(l, m)]);
            ret += shCoeffs[gl_DrawID * NB_SH + flatten(l, m)]
                 * shFuncs[sphVertID * NB_SH + flatten(l, m)];
        }
    }
    if(sum > -1e-10)
    {
        return ret / sum;
    }
    return 0.0;
}

void main()
{
    uint sphVertID = gl_VertexID % nbVertices;
    vec3 position = vec3(vertices[gl_VertexID * 3],
                         vertices[gl_VertexID * 3 + 1],
                         vertices[gl_VertexID * 3 + 2]);

    gl_Position = projectionMatrix
                * viewMatrix
                * modelMatrix[gl_DrawID]
                * vec4(position, 1.0);

    vec3 normal = vec3(normals[gl_VertexID * 3],
                       normals[gl_VertexID * 3 + 1],
                       normals[gl_VertexID * 3 + 2]);
    v_normal = normalize(normal);
    v_color = abs(v_normal);
    v_eye = normalize(eye);
}
