#pragma once

#include <JuceHeader.h>
#include "S3000Types.h"

class KeygroupEncoder
{
public:
    static std::vector<uint8_t> encode(
        const Keygroup& keygroup
    );
};