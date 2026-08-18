#pragma once

#include <vector>
#include <cstdint>
#include "S3000Types.h"
#include "../s3000/Offsets.h"

class ProgramParser
{
public:
    // =========================
    // main entry
    // =========================
    static Program parse(const std::vector<uint8_t>& d);

    static std::vector<uint8_t> serialize(const Program& p);

    // =========================
    // sub parsers
    // =========================
    static Keygroup parseKeygroup(const std::vector<uint8_t>& d);

private:
    // =========================
    // generic read helpers
    // =========================
    template <typename T>
    static T read(const std::vector<uint8_t>& d, size_t offset)
    {
        return static_cast<T>(d[offset]);
    }

    static int8_t readS8(const std::vector<uint8_t>& d, size_t offset);
    static uint16_t readU16(const std::vector<uint8_t>& d, size_t offset);
    static double readTune(
        const std::vector<uint8_t>& d,
        size_t offset);


    static std::string readName(const std::vector<uint8_t>& d, size_t offset, size_t len);

    // =========================
    // internal parsing steps
    // =========================
    static void parseHeader(const std::vector<uint8_t>& d, Program& p);
    static void parseKeygroups(const std::vector<uint8_t>& d, Program& p, const std::map<int, juce::String>& residentSamples);
};
