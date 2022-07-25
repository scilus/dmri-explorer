#version 460
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_shading_language_packing : require

#include "/include/shfield_util.glsl"
#include "/include/orthogrid_util.glsl"

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding=1) buffer allSpheresNormalsBuffer
{
    uint allNormals[];
};

const float FLOAT_EPS = 1e-8;
const float PI = 3.14159265358979323;
const float SIGNED_MAX_UINT8 = 127.0;

vec4 normalizeForPacking(const vec4 v)
{
    return v / SIGNED_MAX_UINT8;
}

vec4 scaleFromUnpacking(const vec4 v)
{
    return v * SIGNED_MAX_UINT8;
}

void updateNormals(uint outputID)
{
    uint firstNormalID = outputID * nbVertices;

    for(uint i = 0; i < nbIndices; i += 3)
    {
        const float rA = readRadius(outputID*nbVertices + indices[i]);
        const float rB = readRadius(outputID*nbVertices + indices[i + 1]);
        const float rC = readRadius(outputID*nbVertices + indices[i + 2]);
        const vec3 a = rA * vertices[indices[i]].xyz;
        const vec3 b = rB * vertices[indices[i + 1]].xyz;
        const vec3 c = rC * vertices[indices[i + 2]].xyz;
        const vec3 ab = normalize(b - a);
        const vec3 ac = normalize(c - a);
        if(length(ab) > FLOAT_EPS && length(ac) > FLOAT_EPS)
        {
            if(abs(dot(ab, ac)) < 1.0)
            {
                const vec4 n = vec4(normalize(cross(ab, ac)), 0.0f);
                const vec4 v0 = scaleFromUnpacking(unpackSnorm4x8(allNormals[indices[i] + firstNormalID]));
                const vec4 v1 = scaleFromUnpacking(unpackSnorm4x8(allNormals[indices[i + 1] + firstNormalID]));
                const vec4 v2 = scaleFromUnpacking(unpackSnorm4x8(allNormals[indices[i + 2] + firstNormalID]));

                allNormals[indices[i] + firstNormalID] = packSnorm4x8(normalizeForPacking(v0 + n));
                allNormals[indices[i + 1] + firstNormalID] = packSnorm4x8(normalizeForPacking(v1 + n));
                allNormals[indices[i + 2] + firstNormalID] = packSnorm4x8(normalizeForPacking(v2 + n));
            }
        }
    }
}

void main()
{
    updateNormals(gl_GlobalInvocationID.x);
}
