#version 460
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_shading_language_packing : require

#include "/include/camera_util.glsl"
#include "/include/shfield_util.glsl"
#include "/include/orthogrid_util.glsl"

layout(std430, binding=1) buffer allNormalsBuffer
{
    uint allNormals[];
};

layout(std430, binding=10) buffer modelTransformsBuffer
{
    mat4 modelMatrix;
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

vec4 getColor()
{
    // direction-encoded colormap
    return abs(vec4(normalize(vertices[gl_VertexID%nbVertices].xyz), 1.0f));
}

void main()
{
    const uint flatVoxID = nonZeroVoxels[gl_DrawID];
    const ivec3 index3d = convertFlatVoxIDTo3DVoxID(flatVoxID);

    mat4 localMatrix;
    localMatrix[0][0] = scaling;
    localMatrix[1][1] = scaling;
    localMatrix[2][2] = scaling;
    localMatrix[3][0] = float(index3d.x - gridDims.x / 2);
    localMatrix[3][1] = float(index3d.y - gridDims.y / 2);
    localMatrix[3][2] = float(index3d.z - gridDims.z / 2);
    localMatrix[3][3] = 1.0f;

    const float radius = readRadius(gl_DrawID*nbVertices+gl_VertexID%nbVertices)
                       * allMaxAmplitude[gl_DrawID];
    const vec4 scaledVertice = vec4(vertices[gl_VertexID%nbVertices].xyz * radius, 1.0f);
    const float isNormalizedf= isNormalized > 0 ? 1.0f : 0.0f;
    const float normalizationFactor = 1.0f;
    const vec4 currentVertex = vec4(scaledVertice.xyz * normalizationFactor, 1.0f);

    gl_Position = projectionMatrix
                * viewMatrix
                * modelMatrix
                * localMatrix
                * currentVertex;

    world_frag_pos = modelMatrix
                   * localMatrix
                   * currentVertex;

    world_normal = modelMatrix
                 * unpackSnorm4x8(allNormals[gl_DrawID*nbVertices + gl_VertexID%nbVertices]);

    color = getColor();
    is_visible = 1.0f;
    world_eye_pos = vec4(eye.xyz, 1.0f);
    vertex_slice = getVertexSlice(index3d);
    fade_enabled = fadeIfHidden > 0 && is3DMode() ? 1.0 : -1.0;
}
