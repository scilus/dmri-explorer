#pragma once
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>

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