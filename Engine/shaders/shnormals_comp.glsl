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

void updateNormals(uint flatVoxID)
{
    uint firstNormalID = flatVoxID * nbVertices;

    // reset normals for sphere
    for(uint i = 0; i < nbVertices; ++i)
    {
        // TODO: Make sure allNormals is zero-initialized
        // so we can remove this.
        allNormals[firstNormalID + i] = vec4(0.0f);
    }

    for(uint i = 0; i < nbIndices; i += 3)
    {
        const float rA = readRadius(flatVoxID*nbVertices + indices[i]);
        const float rB = readRadius(flatVoxID*nbVertices + indices[i + 1]);
        const float rC = readRadius(flatVoxID*nbVertices + indices[i + 2]);
        const vec3 a = rA * vertices[indices[i]].xyz;
        const vec3 b = rB * vertices[indices[i + 1]].xyz;
        const vec3 c = rC * vertices[indices[i + 2]].xyz;
        const vec3 ab = normalize(b - a);
        const vec3 ac = normalize(c - a);
        if(length(ab) > FLOAT_EPS && length(ac) > FLOAT_EPS)
        {
            if(abs(dot(ab, ac)) < 1.0)
            {
                const vec3 n = normalize(cross(ab, ac));
                allNormals[indices[i] + firstNormalID] += vec4(n, 0.0f);
                allNormals[indices[i + 1] + firstNormalID] += vec4(n, 0.0f);
                allNormals[indices[i + 2] + firstNormalID] += vec4(n, 0.0f);
            }
        }
    }
}

void main()
{
    const uint i = gl_GlobalInvocationID.x;
    const uint j = gl_GlobalInvocationID.y;
    const uint k = gl_GlobalInvocationID.z;
    const uint flatVoxID = convertSHCoeffsIndex3DToFlatVoxID(i, j, k);
    updateNormals(flatVoxID);
}
