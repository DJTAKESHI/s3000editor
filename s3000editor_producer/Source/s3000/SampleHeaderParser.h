#pragma once

#include "S3000Types.h"

class SampleHeaderParser
{
public:
    static SampleHeader parse(
        const std::vector<uint8_t>& data,
        int sampleIndex);
};