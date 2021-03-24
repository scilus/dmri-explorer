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