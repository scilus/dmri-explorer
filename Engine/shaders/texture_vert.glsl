#version 460

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;


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

void main()
{
    mat4 localMatrix;
    mat4 otherMatrix;

    otherMatrix[0][0] = 1.0f;
    otherMatrix[1][1] = 1.0f;
    otherMatrix[2][2] = 1.0f;
    otherMatrix[3][0] = float(floor(-gridDims.x/2.0f));
    otherMatrix[3][1] = float(floor(-gridDims.y/2.0f));
    otherMatrix[3][2] = float(floor(-gridDims.z/2.0f));
    otherMatrix[3][3] = 1.0f;

    if(currentSlice == 0) //X
    {
        localMatrix[0][0] = 1.0f;
        localMatrix[1][1] = 1.0f;
        localMatrix[2][2] = 1.0f;
        localMatrix[3][0] = float(sliceIndex.x-floor((gridDims.x/2.0f)));
        localMatrix[3][1] = 0.0f;
        localMatrix[3][2] = 0.0f;
        localMatrix[3][3] = 1.0f;

        frag_tex_coord=vec3(sliceIndex.x/gridDims.x, texCoord.x, texCoord.y);
    }
    if(currentSlice == 1) //Y
    {
        localMatrix[0][0] = 1.0f;
        localMatrix[1][1] = 1.0f;
        localMatrix[2][2] = 1.0f;
        localMatrix[3][0] = 0.0f;
        localMatrix[3][1] = float(sliceIndex.y-floor((gridDims.y/2.0f)));
        localMatrix[3][2] = 0.0f;
        localMatrix[3][3] = 1.0f;

        frag_tex_coord=vec3(texCoord.x, sliceIndex.y/gridDims.y, texCoord.y);
    }
    if(currentSlice == 2) //Z
    {
        localMatrix[0][0] = 1.0f;
        localMatrix[1][1] = 1.0f;
        localMatrix[2][2] = 1.0f;
        localMatrix[3][0] = 0.0f;
        localMatrix[3][1] = 0.0f;
        localMatrix[3][2] = float(sliceIndex.z-floor((gridDims.z/2.0f)));
        localMatrix[3][3] = 1.0f;

        frag_tex_coord=vec3(texCoord, sliceIndex.z/gridDims.z);
    }

        gl_Position = projectionMatrix
                * viewMatrix
                * modelMatrix
                * localMatrix
                * otherMatrix
                * vec4(position,1.0f);

}
