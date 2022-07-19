#version 460
#extension GL_ARB_shading_language_include : require
#extension GL_ARB_shading_language_packing : require

#include "/include/shfield_util.glsl"
#include "/include/orthogrid_util.glsl"

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

const float FLOAT_EPS = 1e-8;
const float PI = 3.14159265358979323;

void scaleSphere(uint flatVoxID, uint outputID)
{
    float sfEval;
    vec3 normal;
    float rmax;
    float maxAmplitude = 0.0f;
    const float sh0 = shCoeffs[flatVoxID * nbCoeffs];
    const bool nonZero = sh0 > FLOAT_EPS;
    // iterate through all sphere directions to find maximum amplitude
    for(uint sphVertID = 0; sphVertID < nbVertices; ++sphVertID)
    {
        sfEval = 0.0f;
        for(int i = 0; i < nbCoeffs; ++i)
        {
            sfEval += shCoeffs[flatVoxID * nbCoeffs + i]
                    * shFuncs[sphVertID * nbCoeffs + i];
        }

        // Evaluate the max amplitude for all vertices.
        maxAmplitude = max(maxAmplitude, sfEval);
    }

    for(uint sphVertID = 0; sphVertID < nbVertices; ++sphVertID)
    {
        zeroInitRadius(outputID*nbVertices + sphVertID);
        if(maxAmplitude > FLOAT_EPS)
        {
            sfEval = 0.0f;
            for(int i = 0; i < nbCoeffs; ++i)
            {
                sfEval += shCoeffs[flatVoxID * nbCoeffs + i]
                        * shFuncs[sphVertID * nbCoeffs + i];
            }
            writeRadius(outputID*nbVertices + sphVertID, sfEval / maxAmplitude);
        }
    }
    maxAmplitude = maxAmplitude > 0.0f ? maxAmplitude : 0.0f;
    allMaxAmplitude[outputID] = maxAmplitude;
}

void main()
{
    const uint flatVoxID = nonZeroVoxels[gl_GlobalInvocationID.x];
    const uint outputID = gl_GlobalInvocationID.x;
    scaleSphere(flatVoxID, outputID);
}
