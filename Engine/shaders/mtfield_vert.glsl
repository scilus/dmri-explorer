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

layout(std430, binding=13) buffer tensorValuesBuffer
{
    mat4 allTensors[];
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

vec4 grayScaleColorMap()
{   
    const float maxAmplitude = allMaxAmplitude[gl_DrawID];
    const float currentRadius = allRadiis[gl_VertexID];
    const vec4 grayScale = vec4(currentRadius/maxAmplitude, currentRadius/maxAmplitude, currentRadius/maxAmplitude, 1.0f);
    return grayScale;
}

vec4 setColorMapMode(vec4 currentVertex)
{
    if (colorMapMode == 1)
    {
        return grayScaleColorMap();
    }
    return abs(vec4(normalize(currentVertex.xyz), 1.0f));
}

void main()
{
    const ivec3 index3d = convertFlatOrthoSlicesIDTo3DVoxID(gl_DrawID % 416);
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

    const uint nbSpheres = gridDims.x*gridDims.y + gridDims.x*gridDims.z + gridDims.y*gridDims.z;
    const uint nbVoxels  = gridDims.x*gridDims.y*gridDims.z;
    mat4 tensorMatrix = allTensors[voxID + nbVoxels*(gl_DrawID/nbSpheres)];//*/

    /*mat4 tensorMatrix;
    tensorMatrix[0][0] = 0.000756478;
    tensorMatrix[1][1] = 0.000765284;
    tensorMatrix[2][2] = 0.000339484;
    tensorMatrix[0][1] = tensorMatrix[1][0] = 0.0;
    tensorMatrix[0][2] = tensorMatrix[2][0] = 0.0;
    tensorMatrix[1][2] = tensorMatrix[2][1] = -0.000112218;
    tensorMatrix[0][3] = tensorMatrix[1][3] = tensorMatrix[2][3] = 0.0f;
    tensorMatrix[3][0] = tensorMatrix[3][1] = tensorMatrix[3][2] = 0.0f;
    tensorMatrix = (1/0.000756478) * tensorMatrix;
    tensorMatrix[3][3] = 1.0f;//*/

    const vec4 currentVertex = vec4(vertices[gl_VertexID%nbVertices].xyz, 1.0f) * tensorMatrix;

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
    is_visible = getIsFlatOrthoSlicesIDVisible(gl_DrawID%nbSpheres) && isAboveThreshold ? 1.0f : -1.0f;
    world_eye_pos = vec4(eye.xyz, 1.0f);
    vertex_slice = getVertexSlice(index3d);
    fade_enabled = fadeIfHidden > 0 && is3DMode() ? 1.0 : -1.0;
}
