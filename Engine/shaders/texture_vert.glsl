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
    localMatrix[0][0] = 1.0f;
    localMatrix[1][1] = 1.0f;
    localMatrix[2][2] = 1.0f;
    localMatrix[3][0] = float(sliceIndex.x-gridDims.x);
    localMatrix[3][1] = float(sliceIndex.y-gridDims.y);
    localMatrix[3][2] = float(sliceIndex.z-gridDims.z);
    localMatrix[3][3] = 1.0f;

    gl_Position = projectionMatrix
                * viewMatrix
                * modelMatrix
                * localMatrix
                * vec4(position,1.0f);

    if(currentSlice == 0) //X
    {
        frag_tex_coord=vec3(sliceIndex.x/gridDims.x, texCoord);
    }
    if(currentSlice == 1) //Y
    {
        frag_tex_coord=vec3(texCoord.x, sliceIndex.y/gridDims.y, texCoord.y);
    }
    if(currentSlice == 2) //Z
    {
        frag_tex_coord=vec3(texCoord, sliceIndex.z/gridDims.z);
    }

}
