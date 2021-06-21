#version 460
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding=0) buffer allSpheresVerticesBuffer
{
    vec4 allVertices[];
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
    uint isNormalized; // bool
    float sh0Threshold;
    float scaling;
};

layout(std430, binding=9) buffer gridInfoBuffer
{
    ivec4 gridDims;
    ivec4 sliceIndex;
    ivec4 isSliceDirty;
};

const uint NB_SH = 45;
const float FLOAT_EPS = 1e-4;

float evaluateSH(uint voxID, uint sphVertID)
{
    float ret = 0.0f;
    float sum = 0.0f;
    for(int i = 0; i < NB_SH; ++i)
    {
        sum += abs(shCoeffs[voxID * NB_SH + i]);
            ret += shCoeffs[voxID * NB_SH + i]
                 * shFuncs[sphVertID * NB_SH + i];
    }
    if(sum > 0.0)
    {
        if(isNormalized > 0)
        {
            return ret / sum;
        }
        return ret;
    }
    return 0.0;
}

void scaleSphere(uint voxID, uint firstVertID, bool isVisible)
{
    float r;
    for(uint i = 0; i < nbVertices; ++i)
    {
        r = isVisible ? evaluateSH(voxID, i) : 0.0f;
        allVertices[firstVertID + i] = vec4(r * vertices[i].xyz, 1.0);
    }
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
        a = allVertices[indices[i] + firstNormalID].xyz;
        b = allVertices[indices[i + 1] + firstNormalID].xyz;
        c = allVertices[indices[i + 2] + firstNormalID].xyz;
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

uint convertInvocationIDToVoxID(uint invocationID)
{
    if(invocationID < gridDims.x * gridDims.y)
    {
        // XY-slice
        const uint j = invocationID / gridDims.x;
        const uint i = invocationID - j * gridDims.x;
        return convertIndex3DToVoxID(i, j, sliceIndex.z);
    }
    if(invocationID < gridDims.x * gridDims.y + gridDims.y * gridDims.z)
    {
        // YZ-slice
        const uint j = (invocationID - gridDims.x * gridDims.y) /gridDims.z;
        const uint k = invocationID - gridDims.x * gridDims.y - j * gridDims.z;
        return convertIndex3DToVoxID(sliceIndex.x, j, k);
    }
    // XZ-slice
    const uint k = (invocationID - gridDims.x * gridDims.y - gridDims.y * gridDims.z) / gridDims.x;
    const uint i = invocationID - gridDims.x * gridDims.y - gridDims.y * gridDims.z - k * gridDims.x;
    return convertIndex3DToVoxID(i, sliceIndex.y, k);
}

bool belongsToXSlice(uint invocationID)
{
    return invocationID >= gridDims.x * gridDims.y &&
           invocationID < gridDims.x * gridDims.y + gridDims.y * gridDims.z;
}

bool belongsToYSlice(uint invocationID)
{
    return invocationID >= gridDims.x * gridDims.y + gridDims.y * gridDims.z;
}

bool belongsToZSlice(uint invocationID)
{
    return invocationID < gridDims.x * gridDims.y;
}

void main()
{
    // current compute shader unit index
    const uint invocationID = gl_GlobalInvocationID.x;
    // first index of sphere in allVertices
    const uint firstVertID = invocationID * nbVertices;

    // condition is slice needs redraw and current processed sphere belongs to said slice
    bool evaluateVoxel = (isSliceDirty.x > 0 && belongsToXSlice(invocationID)) ||
                         (isSliceDirty.y > 0 && belongsToYSlice(invocationID)) ||
                         (isSliceDirty.z > 0 && belongsToZSlice(invocationID));

    if(!evaluateVoxel)
       return; // skip heavy computations

    const uint voxID = convertInvocationIDToVoxID(invocationID);

    bool isVisible = shCoeffs[voxID * NB_SH] > sh0Threshold;

    scaleSphere(voxID, firstVertID, isVisible);
    if(isVisible)
    {
        // skip normal computations for non-visible voxels
        updateNormals(firstVertID);
    }
}
