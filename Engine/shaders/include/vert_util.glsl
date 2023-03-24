/*
Util functions for vertex shader
*/

vec4 getVertexSlice(ivec3 index3d)
{
    const float i = index3d.x == sliceIndex.x ? 1.0f : -1.0f;
    const float j = index3d.y == sliceIndex.y ? 1.0f : -1.0f;
    const float k = index3d.z == sliceIndex.z ? 1.0f : -1.0f;

    return vec4(i, j, k, 0.0f);
}
