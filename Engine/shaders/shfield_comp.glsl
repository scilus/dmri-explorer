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

layout(std430, binding=12) buffer shFuncGradBuffer
{
    vec4 shFuncsGrad[];
};

const float FLOAT_EPS = 1e-4;
const float PI = 3.14159265358979323;

void scaleSphere(uint voxID, uint firstVertID)
{
    float sfEval;
    vec4 shGrad;
    vec3 normal;
    float rmax;
    const float sh0 = shCoeffs[voxID * nbCoeffs];
    for(uint sphVertID = 0; sphVertID < nbVertices; ++sphVertID)
    {
        if(sh0 > FLOAT_EPS)
        {
            sfEval = 0.0f;
            rmax = 0.0f;
            shGrad = vec4(0.0f);
            for(int i = 0; i < nbCoeffs; ++i)
            {
                sfEval += shCoeffs[voxID * nbCoeffs + i]
                        * shFuncs[sphVertID * nbCoeffs + i];

                shGrad += shCoeffs[voxID * nbCoeffs + i]
                        * shFuncsGrad[sphVertID * nbCoeffs + i];

                rmax += (2.0f * L[i] + 1.0f) / 4.0f / PI * pow(shCoeffs[voxID * nbCoeffs + i], 2.0f);
            }

            if(sfEval > FLOAT_EPS)
            {
                // compute normal vector for lighting
                normal = normalize(vertices[sphVertID].xyz - 1.0 / sfEval * shGrad.xyz);
            }
            else
            {
                normal = vec3(0.0f);
            }

            if(isNormalized > 0)
            {
                rmax = sqrt(rmax);
                rmax *= sqrt(0.5f * float(maxOrder) + 1.0f);
                sfEval /= rmax;
            }

            allRadiis[firstVertID + sphVertID] = sfEval;
            allNormals[firstVertID + sphVertID] = vec4(normal, 0.0f);
        }
        else
        {
            allRadiis[firstVertID + sphVertID] = 0.0f;
            allNormals[firstVertID + sphVertID] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
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
    scaleSphere(voxID, firstVertID);
}
