#version 460

layout(std430, binding=0) buffer allRadiisBuffer
{
    float allRadiis[];
};

layout(std430, binding=1) buffer allNormalsBuffer
{
    vec4 allNormals[];
};

layout(std430, binding=3) buffer shCoeffsBuffer
{
    float shCoeffs[];
};

layout(std430, binding=5) buffer sphereVerticesBuffer
{
    vec4 vertices[];
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

layout(std430, binding=9) buffer cameraBuffer
{
    vec4 eye;
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

layout(std430, binding=10) buffer modelTransformsBuffer
{
    mat4 modelMatrix;
};

// Outputs
out gl_PerVertex{
    vec4 gl_Position;
};
out vec4 v_wpos;
out vec3 v_color;
out vec4 v_normal;
out vec4 v_weye;
out float v_isVisible;

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

uint convertIndex3DToVoxID(uint i, uint j, uint k)
{
    return k * gridDims.x * gridDims.y + j * gridDims.x + i;
}

void main()
{
    const ivec3 index3d = convertInvocationIDToIndex3D(gl_DrawID);
    const uint voxID = convertIndex3DToVoxID(index3d.x, index3d.y, index3d.z);
    bool isVisible = shCoeffs[voxID * nbCoeffs] > sh0Threshold;

    mat4 localMatrix;
    localMatrix[0][0] = scaling;
    localMatrix[1][1] = scaling;
    localMatrix[2][2] = scaling;
    localMatrix[3][0] = float(index3d.x - gridDims.x / 2);
    localMatrix[3][1] = float(index3d.y - gridDims.y / 2);
    localMatrix[3][2] = float(index3d.z - gridDims.z / 2);
    localMatrix[3][3] = 1.0;

    vec4 currentVertex = vec4(vertices[gl_VertexID%nbVertices].xyz * allRadiis[gl_VertexID], 1.0f);

    gl_Position = projectionMatrix
                * viewMatrix
                * modelMatrix
                * localMatrix
                * currentVertex;

    v_wpos = modelMatrix
           * localMatrix
           * currentVertex;

    v_normal = modelMatrix
             * allNormals[gl_VertexID];
    v_color = abs(normalize(currentVertex.xyz));
    v_isVisible = isVisible ? 1.0f:0.0f;
    v_weye = eye;
}
