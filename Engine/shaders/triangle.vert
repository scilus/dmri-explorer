#version 460

layout(std430, binding=0) buffer allScaledSpheresBuffer
{
    vec4 allVertices[];
};

layout(std430, binding=1) buffer allNormalsBuffer
{
    vec4 allNormals[];
};

layout(std430, binding=11) buffer modelTransformsBuffer
{
    mat4 modelMatrix;
};

layout(std430, binding=9) buffer gridInfoBuffer
{
    ivec4 gridDims;
    ivec4 sliceIndex;
    ivec4 isSliceDirty;
};

layout(std430, binding=10) buffer cameraBuffer
{
    vec4 eye;
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

layout(std430, binding=8) buffer sphereInfoBuffer
{
    uint nbVertices;
    uint nbIndices;
    uint isNormalized; // bool
    float sh0Threshold;
    float scaling;
};

// Outputs
out gl_PerVertex{
    vec4 gl_Position;
};
out vec3 v_color;
out vec4 v_normal;
out vec4 v_eye;

// Constants
const int NB_SH = 45;

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

ivec3 convertInvocationIDToIndex3D(uint invocationID)
{
    if(invocationID < gridDims.x * gridDims.y)
    {
        // XY-slice
        const uint j = invocationID / gridDims.x;
        const uint i = invocationID - j * gridDims.x;
        return ivec3(i, j, sliceIndex.z);
    }
    if(invocationID < gridDims.x * gridDims.y + gridDims.y * gridDims.z)
    {
        // YZ-slice
        const uint j = (invocationID - gridDims.x * gridDims.y) /gridDims.z;
        const uint k = invocationID - gridDims.x * gridDims.y - j * gridDims.z;
        return ivec3(sliceIndex.x, j, k);
    }
    // XZ-slice
    const uint k = (invocationID - gridDims.x * gridDims.y - gridDims.y * gridDims.z) / gridDims.x;
    const uint i = invocationID - gridDims.x * gridDims.y - gridDims.y * gridDims.z - k * gridDims.x;
    return ivec3(i, sliceIndex.y, k);
}

void main()
{
    const ivec3 index3d = convertInvocationIDToIndex3D(gl_DrawID);
    const uint voxID = convertIndex3DToVoxID(index3d.x, index3d.y, index3d.z);
    mat4 trMat;
    trMat[0][0] = scaling;
    trMat[1][1] = scaling;
    trMat[2][2] = scaling;
    trMat[3][0] = float(index3d.x - gridDims.x / 2);
    trMat[3][1] = float(index3d.y - gridDims.y / 2);
    trMat[3][2] = float(index3d.z - gridDims.z / 2);
    trMat[3][3] = 1.0;

    gl_Position = projectionMatrix
                * viewMatrix
                * modelMatrix
                * trMat
                * allVertices[gl_VertexID];

    v_normal = modelMatrix
             * trMat
             * allNormals[gl_VertexID];
    v_color = abs(normalize(allVertices[gl_VertexID].xyz));
    v_eye = normalize(eye);
}
