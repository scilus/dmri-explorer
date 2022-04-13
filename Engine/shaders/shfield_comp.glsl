#version 460
#extension GL_ARB_shading_language_include : require

#include "/include/shfield_util.glsl"
#include "/include/orthogrid_util.glsl"

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding=0) buffer allRadiisBuffer
{
    float allRadiis[];
};

layout(std430, binding=1) buffer allSpheresNormalsBuffer
{
    vec4 allNormals[];
};

layout(std430, binding=12) buffer allMaxAmplitudeBuffer
{
    float allMaxAmplitude[];
};

const float FLOAT_EPS = 1e-4;
const float PI = 3.14159265358979323;

bool scaleSphere(uint voxID, uint firstVertID)
{
    float sfEval;
    vec3 normal;
    float rmax;
    float maxAmplitude = 0.0f;
    const float sh0 = shCoeffs[voxID * nbCoeffs];
    bool nonZero = sh0 > FLOAT_EPS;
    for(uint sphVertID = 0; sphVertID < nbVertices; ++sphVertID)
    {
        if(nonZero)
        {
            sfEval = 0.0f;
            for(int i = 0; i < nbCoeffs; ++i)
            {
                sfEval += shCoeffs[voxID * nbCoeffs + i]
                        * shFuncs[sphVertID * nbCoeffs + i];
            }

            // Evaluate the max amplitude for all vertices.
            maxAmplitude = max(maxAmplitude, sfEval);
            
            allRadiis[firstVertID + sphVertID] = sfEval;
        }
        else
        {
            allRadiis[firstVertID + sphVertID] = 0.0f;
        }
    }

    maxAmplitude = maxAmplitude > 0.0f ? maxAmplitude : 1.0f;
    allMaxAmplitude[firstVertID / nbVertices] = maxAmplitude;

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

    const uint voxID = convertSHCoeffsIndex3DToFlatVoxID(i, j, k);
    const uint firstVertID = outSphereID * nbVertices;
    if(scaleSphere(voxID, firstVertID))
    {
        updateNormals(firstVertID);
    }
}
