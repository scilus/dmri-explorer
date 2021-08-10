#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <cmath>
#include <iostream>

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
    if (!ifs.is_open())
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
            char log[logLength];
            glGetShaderInfoLog(shader, logLength, NULL, log);
            std::cout << log << std::endl;
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
