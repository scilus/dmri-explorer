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
layout(std430, binding=7) buffer sphereInfoBuffer
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
};
