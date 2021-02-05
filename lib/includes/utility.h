#pragma once

#include <string>
#include <vector>

void makeVector(std::vector<char> &inputVector, const char *data, std::size_t size = -1);
std::string modemStatusBitsToString(unsigned int status, bool add_hex);

