#version 460
layout(local_size_x = 1) in;

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

layout(std430, binding = 7) volatile buffer verticesBuffer
{
    float vertices[]; // {vx, vy, vz, vx, vy, ...}
};

layout(std430, binding = 8) volatile buffer normalsBuffer
{
    float normals[]; // {nx, ny, nz, nx, ny, ...}
};

layout(std430, binding = 9) buffer indicesBuffer
{
    uint indices[]; // {i11, i12, i13, i21, i22, ...}
};

const uint NB_SH = 45;

int flatten(int l, int m)
{
    return l * (l + 1) / 2 + m;
}

float evaluateSH(uint drawID, uint sphVertID)
{
    float ret = 0.0f;
    float sum = 0.0f;
    for(int l = 0; l <= 8; l+=2)
    {
        for(int m = -l; m <= l; ++m)
        {
            sum += abs(shCoeffs[drawID * NB_SH + flatten(l, m)]);
            ret += shCoeffs[drawID * NB_SH + flatten(l, m)]
                 * shFuncs[sphVertID * NB_SH + flatten(l, m)];
        }
    }
    if(sum > 0.0)
    {
        return ret / sum;
    }
    return 1.0;
}

void main()
{
    const uint index =  gl_WorkGroupID.x;
    const uint firstVertID = index * (nbVertices * 3);
    float r;
    uint sphVertID;
    uint vertID;
    for(uint i = 0; i < nbVertices; ++i)
    {
        sphVertID = i;
        vertID = firstVertID + 3 * i;
        r = 0.5; // evaluateSH(index, sphVertID);
        vertices[vertID] *= r;
        vertices[vertID + 1] *= r;
        vertices[vertID + 2] *= r;
    }
}
