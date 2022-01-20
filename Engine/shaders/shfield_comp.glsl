#version 460
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding=0) buffer allRadiisBuffer
{
    float allRadiis[];
};

layout(std430, binding=1) buffer allSpheresNormalsBuffer
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

layout(std430, binding=6) buffer sphereIndicesBuffer
{
    uint indices[];
};

layout(std430, binding=7) buffer sphereInfoBuffer
{
    uint nbVertices;
    uint nbIndices;
    uint isNormalized; // bool
    uint maxOrder;
    float sh0Threshold;
    float scaling;
    uint nbCoeffs;
};

layout(std430, binding=8) buffer gridInfoBuffer
{
    ivec4 gridDims;
    ivec4 sliceIndex;
    uint currentSlice;
};

layout(std430, binding=11) buffer ordersBuffer
{
    float L[];
};

const float FLOAT_EPS = 1e-4;
const float PI = 3.14159265358979323;

bool scaleSphere(uint voxID, uint firstVertID)
{
    float sfEval;
    vec3 normal;
    float rmax;
    const float sh0 = shCoeffs[voxID * nbCoeffs];
    bool nonZero = sh0 > FLOAT_EPS;
    for(uint sphVertID = 0; sphVertID < nbVertices; ++sphVertID)
    {
        if(nonZero)
        {
            sfEval = 0.0f;
            rmax = 0.0f;
            for(int i = 0; i < nbCoeffs; ++i)
            {
                sfEval += shCoeffs[voxID * nbCoeffs + i]
                        * shFuncs[sphVertID * nbCoeffs + i];

                rmax += (2.0f * L[i] + 1.0f) / 4.0f / PI * pow(shCoeffs[voxID * nbCoeffs + i], 2.0f);
            }

            if(isNormalized > 0)
            {
                rmax = sqrt(rmax);
                rmax *= sqrt(0.5f * float(maxOrder) + 1.0f);
                sfEval /= rmax;
            }

            allRadiis[firstVertID + sphVertID] = sfEval;
        }
        else
        {
            allRadiis[firstVertID + sphVertID] = 0.0f;
        }
    }
    return nonZero;
}

void updateNormals(uint firstNormalID)
{
    vec3 ab, ac, n;
    vec3 a, b, c;

    // reset normals for sphere
    for(uint i = 0; i < nbVertices; ++i)
    {
        allNormals[firstNormalID + i] = vec4(0.0, 0.0, 0.0, 0.0);
    }

    for(uint i = 0; i < nbIndices; i += 3)
    {
        a = allRadiis[indices[i] + firstNormalID] * vertices[indices[i]].xyz;
        b = allRadiis[indices[i + 1] + firstNormalID] * vertices[indices[i + 1]].xyz;
        c = allRadiis[indices[i + 2] + firstNormalID] * vertices[indices[i + 2]].xyz;
        ab = b - a;
        ac = c - a;
        if(length(ab) > FLOAT_EPS && length(ac) > FLOAT_EPS)
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

uint convertIndex3DToVoxID(uint i, uint j, uint k)
{
    return k * gridDims.x * gridDims.y + j * gridDims.x + i;
}

void main()
{
    uint i, j, k, outSphereID;
    if(currentSlice == 0) // x-slice
    {
        outSphereID = gl_GlobalInvocationID.x + gridDims.x * gridDims.y;
        i = sliceIndex.x;
        j = gl_GlobalInvocationID.x / gridDims.z;
        k = gl_GlobalInvocationID.x - j * gridDims.z;
    }
    else if(currentSlice == 1) // y-slice
    {
        outSphereID = gl_GlobalInvocationID.x + gridDims.x * gridDims.y + gridDims.y * gridDims.z;
        j = sliceIndex.y;
        k = gl_GlobalInvocationID.x / gridDims.x;
        i = gl_GlobalInvocationID.x - k * gridDims.x;
    }
    else if(currentSlice == 2) // z-slice
    {
        outSphereID = gl_GlobalInvocationID.x;
        k = sliceIndex.z;
        j = gl_GlobalInvocationID.x / gridDims.x;
        i = gl_GlobalInvocationID.x - j * gridDims.x;
    }

    const uint voxID = convertIndex3DToVoxID(i, j, k);
    const uint firstVertID = outSphereID * nbVertices;
    if(scaleSphere(voxID, firstVertID))
    {
        updateNormals(firstVertID);
    }
}
