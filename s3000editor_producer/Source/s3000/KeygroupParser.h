#pragma once
#include <vector>
#include "S3000Types.h"
//#include "Keygroup.h"


class KeygroupParser
{
public:
    static Keygroup parse(const std::vector<uint8_t>& d, const std::map<int, juce::String>& residentSamples);

private:
    static VelocityZone parseZone(
        const std::vector<uint8_t>& d,
        int zoneIndex,
        const std::map<int, juce::String>& residentSamples
    );

    template <typename T>
    static T read(const std::vector<uint8_t>& d, size_t offset)
    {
        return static_cast<T>(d[offset]);
    }

    void KeygroupParser::resolveSampleNames(
        Keygroup& k,
        const std::vector<std::string>& sampleNames);

};



