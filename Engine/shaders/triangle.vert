#version 460

layout (location=0) in vec3 in_pos;
layout (location=1) in vec3 in_color;
layout (location=6) in vec3 in_normal;

// SSBO
layout(std430, binding = 3) buffer instanceTransforms
{
    mat4 modelMatrix[];
};

layout(std430, binding = 4) buffer camera
{
    vec4 eye;
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

layout(std430, binding = 5) buffer drawIndices
{
    uint drawID;
};

// Outputs
out gl_PerVertex{
    vec4 gl_Position;
};
out vec3 v_color;
out vec3 v_normal;
out vec4 v_eye;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix[drawID] * vec4(in_pos, 1.0);
    v_color = in_color;
    v_normal = in_normal;
    v_eye = normalize(eye);
}
