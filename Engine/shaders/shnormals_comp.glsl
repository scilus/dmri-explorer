#version 460
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_shading_language_packing : require

#include "/include/shfield_util.glsl"
#include "/include/orthogrid_util.glsl"

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding=1) buffer allSpheresNormalsBuffer
{
    vec4 allNormals[];
};

const float FLOAT_EPS = 1e-8;
const float PI = 3.14159265358979323;

// TODO: Move to second compute shader.
void updateNormals(uint flatVoxID)
{
    uint firstNormalID = flatVoxID * nbVertices;

    // reset normals for sphere
    for(uint i = 0; i < nbVertices; ++i)
    {
        allNormals[firstNormalID + i] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    for(uint i = 0; i < nbIndices; i += 3)
    {
        const float xxx = readRadius(indices[i] + firstNormalID);
        const float yyy = readRadius(indices[i + 1] + firstNormalID);
        const float zzz = readRadius(indices[i + 2] + firstNormalID);
        const vec3 a =/* readRadius(indices[i] + firstNormalID) **/ vertices[indices[i]].xyz;
        const vec3 b =/* readRadius(indices[i + 1] + firstNormalID) **/ vertices[indices[i + 1]].xyz;
        const vec3 c =/* readRadius(indices[i + 2] + firstNormalID) **/ vertices[indices[i + 2]].xyz;
        const vec3 ab = normalize(b - a);
        const vec3 ac = normalize(c - a);
        if(length(ab) > FLOAT_EPS && length(ac) > FLOAT_EPS)
        {
            if(abs(dot(ab, ac)) < 1.0)
            {
                const vec3 n = normalize(cross(ab, ac));
                allNormals[indices[i] + firstNormalID] += vec4(n, 0.0);
                allNormals[indices[i + 1] + firstNormalID] += vec4(n, 0.0);
                allNormals[indices[i + 2] + firstNormalID] += vec4(n, 0.0);
            }
        }
    }
}

void main()
{
    uint i, j, k;

    i = gl_GlobalInvocationID.x;
    j = gl_GlobalInvocationID.y;
    k = gl_GlobalInvocationID.z;
    const uint flatVoxID = convertSHCoeffsIndex3DToFlatVoxID(i, j, k);
    updateNormals(flatVoxID);
}
