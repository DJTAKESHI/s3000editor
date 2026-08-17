#pragma once

#include <JuceHeader.h>
#include "S3000Types.h"

class SampleHeaderEncoder
{
public:
    static std::vector<uint8_t> encode(
        const SampleHeader& header);

    static std::vector<uint8_t> encodeNibbleData(
        const std::vector<uint8_t>& decoded);

    static std::vector<uint8_t>
        makeSysEx(
            const SampleHeader& header);

};