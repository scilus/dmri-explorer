/*
Util functions for fragment shader
*/

layout(std430, binding=10) buffer modelTransformsBuffer
{
    mat4 modelMatrix;
};

in vec4 vertex_slice;
in vec4 world_frag_pos;
in vec4 world_eye_pos;

const vec4 X_SLICE_NORMAL = vec4(1.0f, 0.0f, 0.0f, 0.0f);
const vec4 Y_SLICE_NORMAL = vec4(0.0f, 1.0f, 0.0f, 0.0f);
const vec4 Z_SLICE_NORMAL = vec4(0.0f, 0.0f, 1.0f, 0.0f);

const float KA = 0.3;
const float KD = 0.6;
const float KS = 0.1;

const float PARALLEL_PLANE_EPSILON = 0.1f;
const float HIDDEN_FRAG_SIGMA = 0.1f;
const float PARALLEL_FRAG_SIGMA = 0.12f;
const float MIN_BG_SHADING = 0.2f;
const float MIN_SHADING_THRESHOLD = 0.1f;

float getNormalized(float x, float cmin, float cmax, float nmin, float nmax)
{
    return (x - cmin)/(cmax - cmin)*(nmax - nmin) + nmin;
}

float transitionFunction(float x, float sigma, float minFading)
{
    const float fx = exp(-0.5f * pow(x / sigma, 2));
    return getNormalized(fx, 0.0f, 1.0f, minFading, 1.0f);
}


float GetFading()
{
    // world coordinate of planes intersection
    const vec4 worldPlanesCenter = modelMatrix
                                 * vec4(sliceIndex.xyz - gridDims.xyz / 2, 1.0f);

    const vec4 planesCenterToFragPosDir = normalize(world_frag_pos - worldPlanesCenter);
    const vec4 planesCenterToEyePosDir = normalize(world_eye_pos - worldPlanesCenter);

    // slices normal
    vec4 worldXNormal = normalize(modelMatrix * X_SLICE_NORMAL);
    vec4 worldYNormal = normalize(modelMatrix * Y_SLICE_NORMAL);
    vec4 worldZNormal = normalize(modelMatrix * Z_SLICE_NORMAL);
    worldXNormal = faceforward(worldXNormal, -worldXNormal, planesCenterToEyePosDir);
    worldYNormal = faceforward(worldYNormal, -worldYNormal, planesCenterToEyePosDir);
    worldZNormal = faceforward(worldZNormal, -worldZNormal, planesCenterToEyePosDir);

    const vec3 absDotEyeNormal = vec3(abs(dot(planesCenterToEyePosDir, worldXNormal)),
                                      abs(dot(planesCenterToEyePosDir, worldYNormal)),
                                      abs(dot(planesCenterToEyePosDir, worldZNormal)));
    const bvec3 isBehindSlice = bvec3(dot(planesCenterToFragPosDir, worldXNormal) < 0.0f &&
                                      absDotEyeNormal.x > PARALLEL_PLANE_EPSILON &&
                                      vertex_slice.x < 0.0f,
                                      dot(planesCenterToFragPosDir, worldYNormal) < 0.0f &&
                                      absDotEyeNormal.y > PARALLEL_PLANE_EPSILON &&
                                      vertex_slice.y < 0.0f,
                                      dot(planesCenterToFragPosDir, worldZNormal) < 0.0f &&
                                      absDotEyeNormal.z > PARALLEL_PLANE_EPSILON &&
                                      vertex_slice.z < 0.0f);

    // test for fragment behind planes
    float backgroundFading = 1.0f;
    if(isBehindSlice.x)
    {
        // object is behind X plane and does not belong to X plane
        const float x = getNormalized(absDotEyeNormal.x, MIN_BG_SHADING, 1.0f, 0.0f, 1.0f);
        backgroundFading = min(backgroundFading,
                               transitionFunction(x, HIDDEN_FRAG_SIGMA, MIN_BG_SHADING));
    }
    if(isBehindSlice.y)
    {
        // object is behind Y plane and does not belong to Y plane
        const float x = getNormalized(absDotEyeNormal.y, MIN_BG_SHADING, 1.0f, 0.0f, 1.0f);
        backgroundFading = min(backgroundFading,
                               transitionFunction(x, HIDDEN_FRAG_SIGMA, MIN_BG_SHADING));
    }
    if(isBehindSlice.z)
    {
        // object is behind Z plane and does not belong to Z plane
        const float x = getNormalized(absDotEyeNormal.z, MIN_BG_SHADING, 1.0f, 0.0f, 1.0f);
        backgroundFading = min(backgroundFading,
                               transitionFunction(x, HIDDEN_FRAG_SIGMA, MIN_BG_SHADING));
    }

    float parallelPlanesFading = 0.0f;
    float newFading;
    // also shade by the angle between the plane and the view vector
    if(vertex_slice.x > 0.0f)
    {
        newFading = 1.0f - transitionFunction(absDotEyeNormal.x, PARALLEL_FRAG_SIGMA, 0.0f);
        parallelPlanesFading = max(parallelPlanesFading, newFading);
    }
    if(vertex_slice.y > 0.0f)
    {
        newFading = 1.0f - transitionFunction(absDotEyeNormal.y, PARALLEL_FRAG_SIGMA, 0.0f);
        parallelPlanesFading = max(parallelPlanesFading, newFading);
    }
    if(vertex_slice.z > 0.0f)
    {
        newFading = 1.0f - transitionFunction(absDotEyeNormal.z, PARALLEL_FRAG_SIGMA, 0.0f);
        parallelPlanesFading = max(parallelPlanesFading, newFading);
    }

    const float fading = min(backgroundFading, parallelPlanesFading);
    if(fading < MIN_SHADING_THRESHOLD)
    {
        discard;
    }

    return fading;
}
