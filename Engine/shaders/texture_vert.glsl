#version 460
#extension GL_ARB_shading_language_include : require

#include "/include/camera_util.glsl"
#include "/include/orthogrid_util.glsl"

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 texCoord;
layout (location = 2) in vec3 slice;

layout(std430, binding=10) buffer modelTransformsBuffer
{
    mat4 modelMatrix;
};

out gl_PerVertex{
    vec4 gl_Position;
};
out vec3 frag_tex_coord;
out float is_visible;


void main()
{
    mat4 localMatrix;
    vec3 direction;

    localMatrix[0][0] = 1.0f;
    localMatrix[1][1] = 1.0f;
    localMatrix[2][2] = 1.0f;
    localMatrix[3][0] = ceil(-gridDims.x/2.0f) - 0.5f;
    localMatrix[3][1] = ceil(-gridDims.y/2.0f) - 0.5f;
    localMatrix[3][2] = ceil(-gridDims.z/2.0f) - 0.5f;
    localMatrix[3][3] = 1.0f;

    is_visible = 1.0f;

    if(slice.x > 0.9f && slice.x < 1.1f)
    {
        frag_tex_coord = vec3(sliceIndex.x/float(gridDims.x-1.0f), texCoord.x, texCoord.y);
        direction = vec3(sliceIndex.x-ceil(gridDims.x/2.0f) + 0.5f, 0.0f, 0.0f);
        if(isSliceVisible.x == 0.0f)
        {
            is_visible = 0.0f;
        }
    }
    if(slice.y > 0.9f && slice.y < 1.1f)
    {
        frag_tex_coord = vec3(texCoord.x, sliceIndex.y/float(gridDims.y-1.0f), texCoord.y);
        direction = vec3(0.0f,sliceIndex.y-ceil(gridDims.y/2.0f) + 0.5f ,0.0f);
        if(isSliceVisible.y == 0.0f)
        {
            is_visible = 0.0f;
        }
    }
    if(slice.z > 0.9f && slice.z < 1.1f)
    {
        frag_tex_coord = vec3(texCoord.x, texCoord.y, sliceIndex.z/float(gridDims.z-1.0f));
        direction = vec3(0.0f, 0.0f, sliceIndex.z-ceil(gridDims.z/2.0f) + 0.5f);
        if(isSliceVisible.z == 0.0f)
        {
            is_visible = 0.0f;
        }
    }

    gl_Position = projectionMatrix
            * viewMatrix
            * modelMatrix
            * localMatrix
            * vec4(position + direction, 1.0f);
}
