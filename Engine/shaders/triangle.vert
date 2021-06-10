#version 460

layout(std430, binding=0) buffer allScaledSpheresBuffer
{
    vec4 allVertices[];
};

layout(std430, binding=1) buffer allNormalsBuffer
{
    vec4 allNormals[];
};

layout(std430, binding=2) buffer instanceTransformsBuffer
{
    mat4 modelMatrix[];
};

layout(std430, binding=9) buffer cameraBuffer
{
    vec4 eye;
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

// Outputs
out gl_PerVertex{
    vec4 gl_Position;
};
out vec3 v_color;
out vec3 v_normal;
out vec4 v_eye;

// Constants
const int NB_SH = 45;

void main()
{
    gl_Position = projectionMatrix
                * viewMatrix
                * modelMatrix[gl_DrawID]
                * allVertices[gl_VertexID];

    v_normal = normalize(allNormals[gl_VertexID].xyz);
    v_color = abs(normalize(allVertices[gl_VertexID].xyz));
    v_eye = normalize(eye);
}
