#version 460
#extension GL_ARB_shading_language_include : require

#include "/include/camera_util.glsl"
#include "/include/shfield_util.glsl"
#include "/include/orthogrid_util.glsl"

layout(std430, binding=0) buffer allRadiisBuffer
{
    float allRadiis[];
};

layout(std430, binding=1) buffer allNormalsBuffer
{
    vec4 allNormals[];
};

layout(std430, binding=10) buffer modelTransformsBuffer
{
    mat4 modelMatrix;
};

layout(std430, binding=12) buffer allMaxAmplitudeBuffer
{
    float allMaxAmplitude[];
};

// Outputs
out gl_PerVertex{
    vec4 gl_Position;
};
out vec4 world_frag_pos;
out vec4 color;
out vec4 world_normal;
out vec4 world_eye_pos;

// Identify the slice a vertex belongs to.
// -1 if the vertex does not belong to slice at index;
// +1 if the vertex belongs to the slice at index.
out vec4 vertex_slice;

// An object is not visible if its SH0 coefficient
// is below the threshold or if the 2D mode is enabled.
out float is_visible;

// Fade is disabled when in 2D!
out float fade_enabled;

vec4 getVertexSlice(ivec3 index3d)
{
    const float i = index3d.x == sliceIndex.x ? 1.0f : -1.0f;
    const float j = index3d.y == sliceIndex.y ? 1.0f : -1.0f;
    const float k = index3d.z == sliceIndex.z ? 1.0f : -1.0f;

    return vec4(i, j, k, 0.0f);
}

float maxAmplitude()
{

    // FIXME: take to much time, new buffer in vertex comp
    uint currentVerticeID = gl_DrawID * nbVertices;
    float max = 0.0;
    for (int i=0; i < nbVertices; ++i)
    {
        const float amplitude = allRadiis[currentVerticeID + i];
        if (amplitude > max)
        {
            max = amplitude;
        }
    }
    return max;
}

vec4 grayScaleColorMap()
{   
    const float maxAmplitude = allMaxAmplitude[gl_DrawID];
    const float currentRadius = allRadiis[gl_VertexID];
    const vec4 grayScale = vec4(currentRadius/maxAmplitude, currentRadius/maxAmplitude, currentRadius/maxAmplitude, 1.0f);
    return grayScale;
}

vec4 setColorMapMode(vec4 currentVertex)
{
    // Default
    if (colorMapMode == 0)
    {
        return abs(vec4(normalize(currentVertex.xyz), 1.0f));
;
    }
    else if (colorMapMode == 1)
    {
        return grayScaleColorMap();
    }
}

void main()
{
    const ivec3 index3d = convertFlatOrthoSlicesIDTo3DVoxID(gl_DrawID);
    const uint voxID = convertSHCoeffsIndex3DToFlatVoxID(index3d.x, index3d.y, index3d.z);
    bool isAboveThreshold = shCoeffs[voxID * nbCoeffs] > sh0Threshold;

    mat4 localMatrix;
    localMatrix[0][0] = scaling;
    localMatrix[1][1] = scaling;
    localMatrix[2][2] = scaling;
    localMatrix[3][0] = float(index3d.x - gridDims.x / 2);
    localMatrix[3][1] = float(index3d.y - gridDims.y / 2);
    localMatrix[3][2] = float(index3d.z - gridDims.z / 2);
    localMatrix[3][3] = 1.0f;

    vec4 currentVertex = vec4(vertices[gl_VertexID%nbVertices].xyz * allRadiis[gl_VertexID], 1.0f);

    gl_Position = projectionMatrix
                * viewMatrix
                * modelMatrix
                * localMatrix
                * currentVertex;

    world_frag_pos = modelMatrix
                   * localMatrix
                   * currentVertex;

    world_normal = modelMatrix
                 * allNormals[gl_VertexID];

    color = setColorMapMode(currentVertex);
    is_visible = getIsFlatOrthoSlicesIDVisible(gl_DrawID) && isAboveThreshold ? 1.0f : -1.0f;
    world_eye_pos = vec4(eye.xyz, 1.0f);
    vertex_slice = getVertexSlice(index3d);
    fade_enabled = fadeIfHidden > 0 && is3DMode() ? 1.0 : -1.0;
}
