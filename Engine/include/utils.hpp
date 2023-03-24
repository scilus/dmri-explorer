#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <limits>
#include <iostream>
#include <cmath>
#include <tuple>
#include <algorithm>

//TODO: Check why M_PI does not work with cmath
# define M_PI           3.14159265358979323846  /* pi */

namespace Slicer
{
const float DEG2RAD_FACTOR = 0.0174532925199f;
const float RAD2DEG_FACTOR = 57.2957795131f;
const int MAX_FACTORIAL = 17;
const double FACTORIALS_LUT[MAX_FACTORIAL] =
{
    1.0,
    1.0,
    2.0,
    6.0,
    24.0,
    120.0,
    720.0,
    5040.0,
    40320.0,
    362880.0,
    3628800.0,
    39916800.0,
    479001600.0,
    6227020800.0,
    87178291200.0,
    1307674368000.0,
    20922789888000.0
};

static inline std::string readFile(const std::string &filePath)
{
    std::ifstream ifs(filePath, std::ifstream::in);
    if(!ifs.is_open())
        throw std::ios_base::failure("cannot open file: " + filePath);

    std::stringstream s;
    s << ifs.rdbuf();
    ifs.close();
    return s.str();
}

static inline const std::string extractPath(const std::string &filePath)
{
    std::size_t found = filePath.find_last_of("/\\");
    if(found != std::string::npos)
    {
        return filePath.substr(0, found+1);
    }
    return "";
}

static inline void assertShaderCompilationSuccess(const GLint shader, const std::string& shaderName)
{
    GLint compileStatus = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if(!compileStatus)
    {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if(logLength > 0)
        {
            char* log = new char[logLength];
            glGetShaderInfoLog(shader, logLength, NULL, log);
            std::cout << log << std::endl;
            delete[] log;
        }
        throw std::runtime_error("Error compiling shader: " + shaderName + ".");
    }
}

static inline void assertProgramLinkingSuccess(const GLint program)
{
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        throw std::runtime_error("Error linking shader program.");
    }
}

static inline float deg2rad(const float& deg)
{
    return DEG2RAD_FACTOR * deg;
}

static inline float rad2deg(const float& rad)
{
    return RAD2DEG_FACTOR * rad;
}

static inline bool doubleEqual(double a, double b)
{
    const double eps = std::numeric_limits<double>().epsilon();
    return a > b - eps && a < b + eps;
}

static inline bool vecEqual(const glm::vec3& v1, const glm::vec3& v2)
{
    const float eps = std::numeric_limits<float>().epsilon();
    return v1.x > v2.x - eps && v1.x < v2.x + eps &&
           v1.y > v2.y - eps && v1.y < v2.y + eps &&
           v1.z > v2.z - eps && v1.z < v2.z + eps;
}

static inline double factorial(int n)
{
    if(n < 0)
    {
        throw std::runtime_error("Invalid value for factorial.");
    }
    return tgamma(n + 1);
}

static inline void logVec3(const glm::vec3& v, const std::string& m)
{
    std::cout << m << " " << v.x << ", " << v.y << ", " << v.z << std::endl;
}

static inline void print(const glm::mat4& mat, const std::string& m)
{
    std::cout << m << std::endl;
    std::cout << mat[0][0] << " " << mat[0][1] << " " << mat[0][2] << " " << mat[0][3] << " " <<std::endl;
    std::cout << mat[1][0] << " " << mat[1][1] << " " << mat[1][2] << " " << mat[1][3] << " " <<std::endl;
    std::cout << mat[2][0] << " " << mat[2][1] << " " << mat[2][2] << " " << mat[2][3] << " " <<std::endl;
    std::cout << mat[3][0] << " " << mat[3][1] << " " << mat[3][2] << " " << mat[3][3] << " " <<std::endl;
}
}

// Returns rotationally invariant parameters of 3D Diffusion Tensor D
static inline glm::vec4 invariants(glm::mat3 D)
{
    double I1 = D[0][0] + D[1][1] + D[2][2];

    double I2 = D[0][0]*D[1][1] + D[1][1]*D[2][2] + D[2][2]*D[0][0] - (
                D[0][1]*D[0][1] + D[0][2]*D[0][2] + D[1][2]*D[1][2]);

    double I3 = D[0][0]*D[1][1]*D[2][2] + 2*D[0][1]*D[0][2]*D[1][2] - (
                D[2][2]*D[0][1]*D[0][1] + D[1][1]*D[0][2]*D[0][2] + D[0][0]*D[1][2]*D[1][2]);
                
    double I4 = D[0][0]*D[0][0] + D[1][1]*D[1][1] + D[2][2]*D[2][2] + 2*(
                D[0][1]*D[0][1] + D[0][2]*D[0][2] + D[1][2]*D[1][2]);

    return glm::vec4(I1, I2, I3, I4);
}

// Returns eigenvalues of 3D tensor D
static inline glm::vec3 eigenvalues(glm::mat3 D)
{
    glm::vec4 I = invariants(D);

    double v = (I.x/3)*(I.x/3) - I.y/3;
    double s = (I.x/3)*(I.x/3)*(I.x/3) - I.x*I.y/6 + I.z/2;
    double o = acos(std::clamp<double>(s / pow(v,3.0/2.0), -1.0, 1.0)) / 3.0;

    double lambda1 = I.x/3 + 2*sqrt(v)*cos(o);
    double lambda2 = I.x/3 - 2*sqrt(v)*cos(M_PI/3 + o);
    double lambda3 = I.x/3 - 2*sqrt(v)*cos(M_PI/3 - o);

    return glm::vec3(lambda1, lambda2, lambda3);
}

// Returns eigenvectors of 3D tensor D
static inline std::tuple<glm::vec3, glm::vec3, glm::vec3> eigenvectors(glm::mat3 D)
{
    glm::vec3 lambdas = eigenvalues(D);

    glm::vec3 A = glm::vec3(D[0][0]) - lambdas;
    glm::vec3 B = glm::vec3(D[1][1]) - lambdas;
    glm::vec3 C = glm::vec3(D[2][2]) - lambdas;

    // check for diagonal tensors
    if (D[0][1]<1e-12 && D[0][2]<1e-12 && D[1][2]<1e-12)
    {
        if (D[0][0]>=D[1][1] && D[0][0]>=D[2][2])
        {
            return std::make_tuple(glm::vec3(1,0,0), glm::vec3(0), glm::vec3(0));
        }
        else if (D[1][1]>=D[0][0] && D[1][1]>=D[2][2])
        {
            return std::make_tuple(glm::vec3(0,1,0), glm::vec3(0), glm::vec3(0));
        }
        else if (D[2][2]>=D[0][0] && D[2][2]>=D[1][1])
        {
            return std::make_tuple(glm::vec3(0,0,1), glm::vec3(0), glm::vec3(0));
        }
    }

    glm::vec3 e[3];
    for (int i=0; i<3; i++)
    {
        e[i] = glm::vec3
        (
            (D[0][1]*D[1][2] - B[i]*D[0][2])*(D[0][2]*D[1][2] - C[i]*D[0][1]),
            (D[0][2]*D[1][2] - C[i]*D[0][1])*(D[0][1]*D[0][2] - A[i]*D[1][2]),
            (D[0][1]*D[0][2] - A[i]*D[1][2])*(D[0][1]*D[1][2] - B[i]*D[0][2])
        );

        e[i] = glm::normalize(e[i]);
    }

    return std::make_tuple(e[0], e[1], e[2]);
}

// Returns fractional anisotropy (FA) of the 3D tensor D
static inline float fractionalAnisotropy(glm::mat3 D)
{
    glm::vec4 I = invariants(D);

    return sqrt(1 - I.y/I.w);
}

// Returns fractional anisotropy (FA) of the lambdas
static inline float fractionalAnisotropy(const glm::vec3& lambdas)
{
    float MD = (lambdas[0] + lambdas[1] + lambdas[2])/3;

    float a = 3*(lambdas[0] - MD)*(lambdas[0] - MD);
    float b = 3*(lambdas[1] - MD)*(lambdas[1] - MD);
    float c = 3*(lambdas[2] - MD)*(lambdas[2] - MD);
    float d = 2*(lambdas[0]*lambdas[0] + lambdas[1]*lambdas[1] + lambdas[2]*lambdas[2]);

    return sqrt( (a+b+c)/d );
}

// Return mean diffusivity (MD) of the lambdas
static inline float meanDiffusivity(const glm::vec3& lambdas)
{
    return (lambdas[0] + lambdas[1] + lambdas[2])/3;
}

// Return axial diffusivity (AD) of the lambdas
static inline float axialDiffusivity(const glm::vec3& lambdas)
{
    return std::fmaxf( lambdas[0], std::fmaxf(lambdas[1], lambdas[2]) );
}

// Return radial diffusivity (RD) of the lambdas
static inline float radialDiffusivity(const glm::vec3& lambdas)
{
    float AD = axialDiffusivity(lambdas);

    if (AD == lambdas[0])
    {
        return (lambdas[1] + lambdas[2])/2;
    }
    else if (AD == lambdas[1])
    {
        return (lambdas[0] + lambdas[2])/2;
    }
    else
    {
        return (lambdas[0] + lambdas[1])/2;
    }
}

// Concatenates 3 column vectors as a matrix
static inline glm::mat3 concatenate(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    glm::mat3 result;

    for (int i=0; i<3; i++)
    {
        result[i][0] = a[i];
        result[i][1] = b[i];
        result[i][2] = c[i];
    }

    return result;
}

// Normalize the values of the array between 0 and 1
static inline void normalize(std::vector<float>& array)
{
    float cmin = array[0], cmax = array[0];

    for(int i=0; i<array.size(); i++)
    {
        cmin = fminf(cmin, array[i]);
        cmax = fmaxf(cmax, array[i]);
    }

    for(int i=0; i<array.size(); i++)
    {
        array[i] = (array[i] - cmin) / (cmax - cmin);
    }
}

static inline glm::mat4 getTensorFromCoefficients(const std::vector<float>& coefficients, size_t offset, std::string format)
{
    glm::mat4 tensor = glm::mat4(1.0f);

    if (format.compare("mrtrix") == 0) // mrtrix -> diagonal format
    {
        tensor[0][0] = coefficients[offset];
        tensor[1][1] = coefficients[offset+1];
        tensor[2][2] = coefficients[offset+2];
        tensor[0][1] = tensor[1][0] = coefficients[offset+3];
        tensor[0][2] = tensor[2][0] = coefficients[offset+4];
        tensor[1][2] = tensor[2][1] = coefficients[offset+5];
    }
    else if (format.compare("dipy") == 0) // lower diagonal format
    {
        tensor[0][0] = coefficients[offset];
        tensor[1][1] = coefficients[offset+2];
        tensor[2][2] = coefficients[offset+5];
        tensor[0][1] = tensor[1][0] = coefficients[offset+1];
        tensor[0][2] = tensor[2][0] = coefficients[offset+3];
        tensor[1][2] = tensor[2][1] = coefficients[offset+4];
    }
    else if (format.compare("fsl") == 0) // upper diagonal format
    {
        tensor[0][0] = coefficients[offset];
        tensor[1][1] = coefficients[offset+3];
        tensor[2][2] = coefficients[offset+5];
        tensor[0][1] = tensor[1][0] = coefficients[offset+1];
        tensor[0][2] = tensor[2][0] = coefficients[offset+2];
        tensor[1][2] = tensor[2][1] = coefficients[offset+4];
    }

    return tensor;
}
