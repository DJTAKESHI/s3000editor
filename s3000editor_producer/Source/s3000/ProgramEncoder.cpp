#include "ProgramEncoder.h"
#include "Offsets.h"

std::vector<uint8_t>
ProgramEncoder::encode(
    const Program& program)
{
    auto data =
        program.rawData;

    if (data.size() < 192)
    {
        DBG(
            "PROGRAM ENCODER: INVALID RAW SIZE = "
            + juce::String((int)data.size())
        );

        return {};
    }

    // Keygroup”‚¾‚¯patch
    data[
        ProgramOffset::Keygroups::NumKeygroups
    ] =
        static_cast<uint8_t>(
            program.groups
            );

        data[
            ProgramOffset::Output::Stereo
        ] =
            static_cast<uint8_t>(
                program.stereoLevel
                );

        return data;
}