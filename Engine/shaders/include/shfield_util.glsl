/*
Global variables for spherical harmonics (SH) field parameters.
*/

const uint N_BITS_PER_SF = 4;
const uint N_VALUES_PER_DTYPE = 32 / N_BITS_PER_SF;
const uint BITMASK = (1 << (N_BITS_PER_SF)) - 1;

/// Compressed SF coefficients. Needs to be accessed with
/// zeroInitRadius, writeRadius and readRadius.
layout(std430, binding=0) buffer allRadiisBuffer
{
    uint allRadiis[];
};

/// Maximum amplitudes of SF.
layout(std430, binding=12) buffer allMaxAmplitudeBuffer
{
    float allMaxAmplitude[];
};

/// Set radius at index to 0.
void zeroInitRadius(uint index)
{
    // 1. convert index to **true index**
    const uint trueIndex = index / N_VALUES_PER_DTYPE;
    const uint bitOffset = index - trueIndex * N_VALUES_PER_DTYPE;
    uint notMask = ~(BITMASK<<(bitOffset*N_BITS_PER_SF));

    // 2. set to 0 using atomicAnd operator
    memoryBarrier();
    atomicAnd(allRadiis[trueIndex], notMask);
}

/// Set radius at index to newValue [0.0, 1.0].
void writeRadius(uint index, float newValue)
{
    const uint trueIndex = index / N_VALUES_PER_DTYPE;
    const uint bitOffset = index - trueIndex * N_VALUES_PER_DTYPE;
    uint value = uint(newValue * float(BITMASK)); // bounded between 0-255
    value = value << (bitOffset*N_BITS_PER_SF);

    memoryBarrier();
    atomicOr(allRadiis[trueIndex], value);
}

float readRadius(uint index)
{
    const uint trueIndex = index / N_VALUES_PER_DTYPE;
    const uint bitOffset = index - trueIndex * N_VALUES_PER_DTYPE;
    memoryBarrier();
    const uint packedRadius = allRadiis[trueIndex];

    const uint mask = BITMASK << (bitOffset*N_BITS_PER_SF);
    const uint radius = (packedRadius & mask) >> (N_BITS_PER_SF*bitOffset);

    return float(radius) / float(BITMASK);
}

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

/// Sphere vertices buffer.
layout(std430, binding=5) buffer sphereVerticesBuffer
{
    /// Vertices defining the sphere used for glyphs visualization.
    vec4 vertices[];
};

/// Sphere indices buffer.
layout(std430, binding=6) buffer sphereIndicesBuffer
{
    /// Vertex indices defining the sphere triangulation.
    uint indices[];
};

/// Sphere parameters buffer.
layout(std430, binding=7) readonly buffer sphereInfoBuffer
{
    /// Number of vertices.
    uint nbVertices;

    /// Number of indices (3 x number of triangles).
    uint nbIndices;

    /// Is the sphere normalized (L-max norm)? 1 or 0.
    uint isNormalized;

    /// The maximum SH order for the truncated SH series.
    uint maxOrder;

    /// Threshold on 0th SH coefficient. Voxels below the
    /// threshold may be discarded.
    float sh0Threshold;

    /// The scaling factor for SH glyphs.
    float scaling;

    /// Number of SH coefficients.
    uint nbCoeffs;

    /// 1 or 0. Flag for fading out glyphs that are below another slice.
    uint fadeIfHidden;
    
    /// Current color map mode. Default to 0 (RGB).
    uint colorMapMode;
};
