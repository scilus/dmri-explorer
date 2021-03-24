#pragma once

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

static inline std::string extractPath(const std::string &filePath)
{
    std::size_t found = filePath.find_last_of("/\\");
    if(found != std::string::npos)
    {
        return filePath.substr(0, found);
    }
    return "";
}