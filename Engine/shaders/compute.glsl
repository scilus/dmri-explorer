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
    ivec4 isSliceDirty;
};

layout(std430, binding=11) buffer ordersBuffer
{
    float L[];
};

layout(std430, binding=12) buffer shFuncGradBuffer
{
    vec4 shFuncsGrad[];
};

const float FLOAT_EPS = 1e-4;
const float PI = 3.14159265358979323;

void scaleSphere(uint voxID, uint firstVertID)
{
    float rmax = 0.0f;
    for(int i = 0; i < nbCoeffs; ++i)
    {
        rmax += (2.0f * L[i] + 1.0f) / 4.0f / PI * pow(shCoeffs[voxID * nbCoeffs + i], 2);
    }

    float shEval;
    vec4 shGrad;
    vec3 normal;
    for(uint sphVertID = 0; sphVertID < nbVertices; ++sphVertID)
    {
        if(rmax > FLOAT_EPS)
        {
            shEval = 0.0f;
            shGrad = vec4(0.0, 0.0, 0.0, 0.0);
            for(int i = 0; i < nbCoeffs; ++i)
            {
                shEval += shCoeffs[voxID * nbCoeffs + i]
                        * shFuncs[sphVertID * nbCoeffs + i];
                
                shGrad += shCoeffs[voxID * nbCoeffs + i]
                        * shFuncsGrad[sphVertID * nbCoeffs + i];
            }

            if(isNormalized > 0)
            {
                rmax = sqrt(rmax);
                rmax *= sqrt(0.5f * float(maxOrder) + 1);
                shEval = shEval / rmax;
            }

            if(shEval > FLOAT_EPS)
            {
                // compute normal vector for lighting
                normal = normalize(vertices[sphVertID].xyz - 1.0 / shEval * shGrad.xyz);
            }
            else
            {
                normal = vec3(0.0, 0.0, 0.0);
            }

            allRadiis[firstVertID + sphVertID] = shEval;
            allNormals[firstVertID + sphVertID] = vec4(normal, 0.0);
        }
        else
        {
            allRadiis[firstVertID + sphVertID] = 0.0f;
            allNormals[firstVertID + sphVertID] = vec4(0.0, 0.0, 0.0, 0.0);
        }
    }
}

uint convertIndex3DToVoxID(uint i, uint j, uint k)
{
    return k * gridDims.x * gridDims.y + j * gridDims.x + i;
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

uint convertInvocationIDToVoxID(uint invocationID)
{
    if(belongsToZSlice(invocationID))
    {
        // XY-slice
        const uint j = invocationID / gridDims.x;
        const uint i = invocationID - j * gridDims.x;
        return convertIndex3DToVoxID(i, j, sliceIndex.z);
    }
    if(belongsToXSlice(invocationID))
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

void main()
{
    // current compute shader unit index
    const uint invocationID = gl_GlobalInvocationID.x;
    // first index of sphere in allRadiis and allNormals
    const uint firstVertID = invocationID * nbVertices;

    // condition is slice needs redraw and current processed sphere belongs to said slice
    bool evaluateVoxel = (isSliceDirty.x > 0 && belongsToXSlice(invocationID)) ||
                         (isSliceDirty.y > 0 && belongsToYSlice(invocationID)) ||
                         (isSliceDirty.z > 0 && belongsToZSlice(invocationID));

    if(!evaluateVoxel)
       return; // skip heavy computations

    const uint voxID = convertInvocationIDToVoxID(invocationID);
    scaleSphere(voxID, firstVertID);
}
