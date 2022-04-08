#version 460
#extension GL_ARB_shading_language_include : require

#include "/include/camera_util.glsl"
#include "/include/orthogrid_util.glsl"

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 texCoord;
layout (location = 2) in vec3 slice;

// Outputs
out gl_PerVertex{
    vec4 gl_Position;
};
out vec3 frag_tex_coord;

void main()
{
    mat4 localMatrix;
    vec3 direction;

    localMatrix[0][0] = 1.0f;
    localMatrix[1][1] = 1.0f;
    localMatrix[2][2] = 1.0f;
    localMatrix[3][0] = float(floor(-gridDims.x/2.0f)) + 0.5f;
    localMatrix[3][1] = float(floor(-gridDims.y/2.0f)) + 0.5f;
    localMatrix[3][2] = float(floor(-gridDims.z/2.0f)) + 0.5f;
    localMatrix[3][3] = 1.0f;

    if(slice.x == 1)
    {
        frag_tex_coord = vec3(sliceIndex.x/float(gridDims.x), texCoord.x, texCoord.y);
        direction = vec3(sliceIndex.x-floor(gridDims.x/2.0f) - 0.5f, 0.0f, 0.0f);
    }
    if(slice.y == 1)
    {
        frag_tex_coord = vec3(texCoord.x, sliceIndex.y/float(gridDims.y), texCoord.y);
        direction = vec3(0.0f,sliceIndex.y-floor(gridDims.y/2.0f) - 0.5f ,0.0f);

    }
    if(slice.z == 1)
    {
        frag_tex_coord = vec3(texCoord.x, texCoord.y, sliceIndex.z/float(gridDims.z));
        direction = vec3(0.0f, 0.0f, sliceIndex.z-floor(gridDims.z/2.0f) - 0.5f);
    }

    gl_Position = projectionMatrix
            * viewMatrix
            * modelMatrix
            * localMatrix
            * vec4(position + direction, 1.0f);
}
