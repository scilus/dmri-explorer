#version 460

layout (location=0) in vec3 in_pos;
layout (location=1) in vec3 in_color;
layout (location=2) in vec3 in_normal;

// SSBO
layout(std430, binding = 3) buffer MVP
{
    vec3 data_SSBO[];
};

out gl_PerVertex{
    vec4 gl_Position;
};
out vec3 v_color;

void main()
{
    gl_Position = vec4(in_pos, 1.0);
    v_color = data_SSBO[0];
}