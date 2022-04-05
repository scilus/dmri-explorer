/*
Camera parameters.
*/

/// Camera parameters buffer.
layout(std430, binding=9) buffer cameraBuffer
{
    /// Eye position in world coordinates.
    vec4 eye;

    /// View matrix tranforming world coordinates
    /// to camera coordinates.
    mat4 viewMatrix;

    /// Projection matrix.
    mat4 projectionMatrix;
};
