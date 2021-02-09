#pragma once

#include <string>
#include <vector>

void makeVector(std::vector<char> &inputVector, const char *data, std::size_t size = -1);

namespace OutputBitsFormat
{
    constexpr uint8_t eNone = 0;
    constexpr uint8_t eShowHex = 1;
    constexpr uint8_t eShowZeroBits = 2;
    const std::string EMPTY = "---";
}
std::string modemStatusBitsToString(unsigned int status, uint8_t options = OutputBitsFormat::eNone);

