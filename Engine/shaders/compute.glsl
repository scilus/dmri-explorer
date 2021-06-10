#version 460
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding=0) buffer allScaledSpheresBuffer
{
    vec4 allVertices[];
};

layout(std430, binding=1) buffer allNormalsBuffer
{
    vec4 allNormals[];
};

layout(std430, binding=3) buffer shCoeffsBuffer
{
    float shCoeffs[];
};

layout(std430, binding=4) buffer shFunctionsBuffer
{
    float shFuncs[];
};

layout(std430, binding=5) buffer sphereVerticesBuffer
{
    vec4 vertices[];
};

layout(std430, binding=6) buffer sphereNormalsBuffer
{
    vec4 normals[];
};

layout(std430, binding=7) buffer sphereIndicesBuffer
{
    uint indices[];
};

layout(std430, binding=8) buffer sphereInfoBuffer
{
    uint nbVertices;
    uint nbIndices;
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
    return 0.0;
}

void scaleSphere(uint voxID, uint firstVertID)
{
    float r;
    for(uint i = 0; i < nbVertices; ++i)
    {
        r = evaluateSH(voxID, i);
        allVertices[firstVertID + i] = vec4(r * vertices[i].xyz, 1.0);
    }
}

void updateNormals(uint firstNormalID)
{
    vec3 ab, ac, n;
    vec3 a, b, c;

    for(uint i = 0; i < nbIndices; i += 3)
    {
        a = allVertices[indices[i] + firstNormalID].xyz;
        b = allVertices[indices[i + 1] + firstNormalID].xyz;
        c = allVertices[indices[i + 2] + firstNormalID].xyz;
        ab = b - a;
        ac = c - a;
        if(length(ab) > 1e-4 && length(ac) > 1e-4)
        {
            ab = normalize(ab);
            ac = normalize(ac);
            if(abs(dot(ab, ac)) < 1.0)
            {
                n = normalize(cross(ab, ac));
                allNormals[indices[i] + firstNormalID] += vec4(n, 0.0);
                allNormals[indices[i + 1] + firstNormalID] += vec4(n, 0.0);
                allNormals[indices[i + 2] + firstNormalID] += vec4(n, 0.0);
            }
        }
    }
}

void main()
{
    // voxel index; all unit scales one sphere
    const uint voxID = gl_GlobalInvocationID.x;
    // first index of sphere in allVertices
    const uint firstVertID = voxID * nbVertices;
    const uint firstNormalID = firstVertID;

    scaleSphere(voxID, firstVertID);
    updateNormals(firstVertID);
}
