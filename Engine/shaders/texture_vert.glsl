#version 460

layout(std430, binding=8) buffer gridInfoBuffer
{
    ivec4 gridDims;
    ivec4 sliceIndex;
    uint currentSlice;
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

layout(location = 0) in vec4 a_vec;  // Vertex (Un coin du plan) 

// Outputs
out gl_PerVertex{
    vec4 gl_Position;
};
out vec4 world_frag_pos;
out vec4 color;
out vec4 world_normal;
out vec4 world_eye_pos;

const vec4 X_SLICE_NORMAL = vec4(1.0f, 0.0f, 0.0f, 0.0f);
const vec4 Y_SLICE_NORMAL = vec4(0.0f, 1.0f, 0.0f, 0.0f);
const vec4 Z_SLICE_NORMAL = vec4(0.0f, 0.0f, 1.0f, 0.0f);

void main()
{
    vec4 currentVertex = vec4(a_vec.xyz, 1.0f);

    gl_Position = projectionMatrix
                * viewMatrix
                * modelMatrix
                * currentVertex;

    world_frag_pos = modelMatrix
                   * currentVertex;

    world_normal = X_SLICE_NORMAL; //À changer selon la slice

    color = abs(vec4(normalize(currentVertex.xyz), 1.0f));
    world_eye_pos = vec4(eye.xyz, 1.0f);
}
