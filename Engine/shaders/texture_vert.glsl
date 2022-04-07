#version 460

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 texCoord;
layout (location = 2) in vec3 slice;


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

layout(std430, binding=8) buffer gridInfoBuffer
{
    ivec4 gridDims;
    ivec4 sliceIndex;
    uint currentSlice;
};

// Outputs
out gl_PerVertex{
    vec4 gl_Position;
};
out vec3 frag_tex_coord;
out vec4 color;

void main()
{
    mat4 localMatrix;
    mat4 otherMatrix;
    vec3 direction;

    otherMatrix[0][0] = 1.0f;
    otherMatrix[1][1] = 1.0f;
    otherMatrix[2][2] = 1.0f;
    otherMatrix[3][0] = float(floor(-gridDims.x/2.0f));
    otherMatrix[3][1] = float(floor(-gridDims.y/2.0f));
    otherMatrix[3][2] = float(floor(-gridDims.z/2.0f));
    otherMatrix[3][3] = 1.0f;

    frag_tex_coord=texCoord;

    direction = vec3(slice.x*(sliceIndex.x-floor((gridDims.x/2.0f))),slice.y*(sliceIndex.y-floor((gridDims.y/2.0f))),slice.z*(sliceIndex.z-floor((gridDims.z/2.0f))));
    gl_Position = projectionMatrix
            * viewMatrix
            * modelMatrix
            * otherMatrix
            * vec4(position+direction,1.0f);
}
