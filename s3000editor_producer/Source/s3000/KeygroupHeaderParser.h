#pragma once

#include "S3000Types.h"
#include <vector>


class KeygroupHeaderParser
{
public:

    static KeygroupHeader parse(
        const std::vector<uint8_t>& d
    );
};