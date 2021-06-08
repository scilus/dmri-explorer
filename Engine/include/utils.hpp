#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <iostream>

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

static inline glm::vec3 sphericalToCartesian(const float r,
                                             const float theta,
                                             const float phi)
{
    glm::vec3 pos;
    pos.x = r * sin(phi) * sin(theta);
    pos.y = r * cos(theta);
    pos.z = r * cos(phi) * sin(theta);

    return pos;
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
    if(n < 0 || n >= MAX_FACTORIAL)
    {
        std::cout << "Factorial n value: " << n << std::endl;
        throw std::runtime_error("Invalid value for factorial.");
    }

    return FACTORIALS_LUT[n];
}

static inline void logVec3(const glm::vec3& v, const std::string& m)
{
    std::cout << m << " " << v.x << ", " << v.y << ", " << v.z << std::endl;
}
