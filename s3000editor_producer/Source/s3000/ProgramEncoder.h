#pragma once

#include <vector>
#include <cstdint>

#include "S3000Types.h"

class ProgramEncoder
{
public:
    static std::vector<uint8_t> encode(
        const Program& program);
};