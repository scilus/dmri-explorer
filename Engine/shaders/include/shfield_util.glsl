/*
Global variables for spherical harmonics (SH) field parameters.
*/

/// SH coefficients image buffer.
layout(std430, binding=3) buffer shCoeffsBuffer
{
    /// Flattened array of SH coefficients for a whole image.
    float shCoeffs[];
};

/// SH functions buffer.
layout(std430, binding=4) buffer shFunctionsBuffer
{
    /// Table of all SH functions evaluated for each sphere
    /// direction on the sphere to use for rendering.
    float shFuncs[];
};

/// SH orders buffer.
layout(std430, binding=11) buffer ordersBuffer
{
    /// Array of length equal to the number of SH coefficients containing the
    /// integer order for each SH coefficient.
    float L[];
};
