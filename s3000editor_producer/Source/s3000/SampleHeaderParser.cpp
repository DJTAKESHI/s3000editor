#include <JuceHeader.h>
#include "SampleHeaderParser.h"
#include "Offsets.h"

namespace
{
    uint16_t readUInt16(
        const std::vector<uint8_t>& d,
        size_t offset)
    {
        return
            static_cast<uint16_t>(d[offset]) |
            (static_cast<uint16_t>(d[offset + 1]) << 8);
    }


    uint32_t readUInt32(
        const std::vector<uint8_t>& d,
        size_t offset)
    {
        return
            static_cast<uint32_t>(d[offset]) |
            (static_cast<uint32_t>(d[offset + 1]) << 8) |
            (static_cast<uint32_t>(d[offset + 2]) << 16) |
            (static_cast<uint32_t>(d[offset + 3]) << 24);
    }

    uint64_t readUInt48(
        const std::vector<uint8_t>& d,
        size_t offset)
    {
        return
            static_cast<uint64_t>(d[offset]) |
            (static_cast<uint64_t>(d[offset + 1]) << 8) |
            (static_cast<uint64_t>(d[offset + 2]) << 16) |
            (static_cast<uint64_t>(d[offset + 3]) << 24) |
            (static_cast<uint64_t>(d[offset + 4]) << 32) |
            (static_cast<uint64_t>(d[offset + 5]) << 40);
    }

    double readFixed16_16(
        const std::vector<uint8_t>& d,
        size_t offset)
    {
        uint32_t raw =
            static_cast<uint32_t>(d[offset]) |
            (static_cast<uint32_t>(d[offset + 1]) << 8) |
            (static_cast<uint32_t>(d[offset + 2]) << 16) |
            (static_cast<uint32_t>(d[offset + 3]) << 24);

        return static_cast<double>(raw) / 65536.0;
    }

    double readFixed32_16(
        const std::vector<uint8_t>& d,
        size_t offset)
    {
        uint64_t raw =
            static_cast<uint64_t>(d[offset]) |
            (static_cast<uint64_t>(d[offset + 1]) << 8) |
            (static_cast<uint64_t>(d[offset + 2]) << 16) |
            (static_cast<uint64_t>(d[offset + 3]) << 24) |
            (static_cast<uint64_t>(d[offset + 4]) << 32) |
            (static_cast<uint64_t>(d[offset + 5]) << 40);

        return static_cast<double>(raw) / 65536.0;
    }
    double readTune(
        const std::vector<uint8_t>& d,
        size_t offset)
    {
        const int8_t semitone =
            static_cast<int8_t>(d[offset + 1]);

        const double fraction =
            static_cast<double>(d[offset]) / 256.0;

        return static_cast<double>(semitone) + fraction;
    }
}


static char decodePlistChar(uint8_t v)
{
    if (v <= 9)
        return '0' + v;

    if (v == 10)
        return ' ';

    if (v >= 11 && v <= 36)
        return 'A' + (v - 11);

    if (v == 37)
        return '#';

    if (v == 38)
        return '+';

    if (v == 39)
        return '-';

    if (v == 40)
        return '.';

    return '?';
}

SampleHeader SampleHeaderParser::parse(
    const std::vector<uint8_t>& d,
    int sampleIndex)
{
    SampleHeader s{};

    // éÛêMÇµÇΩ141 bytesÇÇªÇÃÇ‹Ç‹ï€ë∂
    s.rawData = d;

    s.id = sampleIndex;
    


    if (d.size() < 132)
    {
        DBG("Sample header too small");
        return s;
    }


    s.id = sampleIndex;





    s.bandwidth =
        d[SampleHeaderOffset::SBANDW];


    s.originalPitch =
        d[SampleHeaderOffset::SPITCH];






    for (int i = 0; i < 12; i++)
    {
        s.name += decodePlistChar(
            d[SampleHeaderOffset::SHNAME + i]
        );
    }


    s.sampleRateValid =
        (d[SampleHeaderOffset::SSRVLD] == 0x80);


    s.numLoops =
        d[SampleHeaderOffset::SLOOPS];

    s.activeLoop =
        d[SampleHeaderOffset::SALOOP];

    s.highestLoop =
        d[SampleHeaderOffset::SHLOOP];

    s.playType =
        d[SampleHeaderOffset::SPTYPE];

    s.tune =
        readTune(
            d,
            SampleHeaderOffset::STUNO
        );




    constexpr size_t firstLoopOffset = 38;
    constexpr size_t loopStride = 12;

    for (int i = 0; i < 4; ++i)
    {
        const size_t base = firstLoopOffset + i * loopStride;

        s.loops[i].position = readUInt32(d, base);
        s.loops[i].length = readFixed32_16(d, base + 4);
        s.loops[i].dwell = readUInt16(d, base + 10);
    }

    s.location =readUInt32(d, SampleHeaderOffset::SLOCAT);



    s.length =readUInt32(d, SampleHeaderOffset::SLNGTH);


    s.start =readUInt32(d, SampleHeaderOffset::SSTART);

    s.end =readUInt32(d, SampleHeaderOffset::SMPEND);

    s.loops[0].relativeFactors =
        readUInt32(d, SampleHeaderOffset::SLXY1);

    s.loops[1].relativeFactors =
        readUInt32(d, SampleHeaderOffset::SLXY2);

    s.loops[2].relativeFactors =
        readUInt32(d, SampleHeaderOffset::SLXY3);

    s.loops[3].relativeFactors =
        readUInt32(d, SampleHeaderOffset::SLXY4);

    s.spare =
        d[SampleHeaderOffset::SSPARE];

    s.waveComment =
        d[SampleHeaderOffset::SWCOMM];

    s.stereoPartner =
        readUInt16(
            d,
            SampleHeaderOffset::SSPAIR
        );

    s.sampleRate =
        readUInt16(
            d,
            SampleHeaderOffset::SSRATE
        );

    s.holdLoopTune =
        static_cast<int8_t>(
            d[SampleHeaderOffset::SHLTO]
            );

    

    constexpr size_t firstSlxyOffset = 86;
    constexpr size_t slxyStride = 12;

    for (int i = 0; i < 4; ++i)
    {
        s.loops[i].relativeFactors =
            readUInt32(
                d,
                firstSlxyOffset + i * slxyStride
            );
    }


    return s;
}

