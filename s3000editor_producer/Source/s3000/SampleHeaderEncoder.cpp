#include "SampleHeaderEncoder.h"
#include "Offsets.h"

void writeUInt32(
    std::vector<uint8_t>& d,
    size_t offset,
    uint32_t value)
{
    d[offset] =
        static_cast<uint8_t>(value & 0xFF);

    d[offset + 1] =
        static_cast<uint8_t>((value >> 8) & 0xFF);

    d[offset + 2] =
        static_cast<uint8_t>((value >> 16) & 0xFF);

    d[offset + 3] =
        static_cast<uint8_t>((value >> 24) & 0xFF);
}

void writeFixed32_16(
    std::vector<uint8_t>& d,
    size_t offset,
    double value)
{
    const uint64_t raw =
        static_cast<uint64_t>(
            std::llround(value * 65536.0)
            );

    d[offset] = static_cast<uint8_t>(raw & 0xFF);
    d[offset + 1] = static_cast<uint8_t>((raw >> 8) & 0xFF);
    d[offset + 2] = static_cast<uint8_t>((raw >> 16) & 0xFF);
    d[offset + 3] = static_cast<uint8_t>((raw >> 24) & 0xFF);
    d[offset + 4] = static_cast<uint8_t>((raw >> 32) & 0xFF);
    d[offset + 5] = static_cast<uint8_t>((raw >> 40) & 0xFF);
}

void writeUInt16(
    std::vector<uint8_t>& d,
    size_t offset,
    uint16_t value)
{
    d[offset] =
        static_cast<uint8_t>(value & 0xFF);

    d[offset + 1] =
        static_cast<uint8_t>((value >> 8) & 0xFF);
}


namespace
{
    uint8_t encodePlistChar(char c)
    {
        if (c >= '0' && c <= '9')
            return static_cast<uint8_t>(c - '0');

        if (c == ' ')
            return 10;

        if (c >= 'A' && c <= 'Z')
            return static_cast<uint8_t>(11 + (c - 'A'));

        if (c == '#')
            return 37;

        if (c == '+')
            return 38;

        if (c == '-')
            return 39;

        if (c == '.')
            return 40;

        return 10;
    }
}


std::vector<uint8_t>
SampleHeaderEncoder::encode(const SampleHeader& s)
{
    std::vector<uint8_t> d = s.rawData;

    if (d.size() != SampleHeaderFormat::Size)
    {
        d.resize(
            SampleHeaderFormat::Size,
            0
        );
    }

    // ==============================
    // Header ID
    // ==============================

    d[SampleHeaderOffset::SHIDENT] = 0x03;


    // ==============================
    // Bandwidth
    // ==============================

    DBG(
        "ENCODE BANDWIDTH = "
        + juce::String(s.bandwidth)
    );

    d[SampleHeaderOffset::SBANDW] =
        static_cast<uint8_t>(s.bandwidth);


    // ==============================
    // Original Pitch
    // ==============================

    d[SampleHeaderOffset::SPITCH] =
        static_cast<uint8_t>(s.originalPitch);


    // ==============================
    // Name
    // ==============================

    for (int i = 0; i < 12; ++i)
    {
        uint8_t value = 10; // space

        if (i < s.name.length())
        {
            value = encodePlistChar(s.name[i]);
        }

        d[SampleHeaderOffset::SHNAME + i] = value;
    }


    // ==============================
    // Sample Rate Valid
    // ==============================

    d[SampleHeaderOffset::SSRVLD] =
        s.sampleRateValid ? 0x80 : 0x00;


    // ==============================
    // Number of loops
    // ==============================

    d[SampleHeaderOffset::SLOOPS] =
        static_cast<uint8_t>(s.numLoops);


    // ==============================
    // Play Type
    // ==============================

    DBG(
        "ENCODE PLAYBACK TYPE = "
        + juce::String(s.playType)
    );


    d[SampleHeaderOffset::SPTYPE] =
        static_cast<uint8_t>(s.playType);

    d[SampleHeaderOffset::SALOOP] =
        static_cast<uint8_t>(s.activeLoop);

    d[SampleHeaderOffset::SHLOOP] =
        static_cast<uint8_t>(s.highestLoop);


    // ==============================
    // Tune
    // ==============================

    const int semitone =
        static_cast<int>(std::floor(s.tune));

    const double fraction =
        s.tune - static_cast<double>(semitone);

    d[SampleHeaderOffset::STUNO] =
        static_cast<uint8_t>(
            std::round(fraction * 256.0)
            );

    d[SampleHeaderOffset::STUNO + 1] =
        static_cast<uint8_t>(
            static_cast<int8_t>(semitone)
            );

    DBG(
        juce::String::formatted(
            "ENCODE STUNO = %02X %02X",
            (unsigned int)d[SampleHeaderOffset::STUNO],
            (unsigned int)d[SampleHeaderOffset::STUNO + 1]
        )
    );

    DBG(
        "ENCODE TUNE VALUE = "
        + juce::String(s.tune, 2)
    );

    d[SampleHeaderOffset::SHLTO] =
        static_cast<uint8_t>(
            s.holdLoopTune
            );

    DBG(
        "ENCODE SHLTO = 0x"
        + juce::String::toHexString(
            d[SampleHeaderOffset::SHLTO]
        )
    );

    DBG(
        "ENCODE HOLD LOOP TUNE = "
        + juce::String((int)s.holdLoopTune)
    );

    writeUInt32(
        d,
        SampleHeaderOffset::SSTART,
        s.start
    );

    writeUInt32(
        d,
        SampleHeaderOffset::SLNGTH,
        static_cast<uint32_t>(s.length)
    );

    DBG(
        "ENCODE START = "
        + juce::String((juce::int64)s.start)
    );

    writeUInt32(
        d,
        SampleHeaderOffset::SMPEND,
        s.end
    );

    constexpr size_t loopOffsets[4][3] =
    {
        {
            SampleHeaderOffset::Loop1::LOOPAT,
            SampleHeaderOffset::Loop1::LLNGTH,
            SampleHeaderOffset::Loop1::LDWELL
        },
        {
            SampleHeaderOffset::Loop2::LOOPAT,
            SampleHeaderOffset::Loop2::LLNGTH,
            SampleHeaderOffset::Loop2::LDWELL
        },
        {
            SampleHeaderOffset::Loop3::LOOPAT,
            SampleHeaderOffset::Loop3::LLNGTH,
            SampleHeaderOffset::Loop3::LDWELL
        },
        {
            SampleHeaderOffset::Loop4::LOOPAT,
            SampleHeaderOffset::Loop4::LLNGTH,
            SampleHeaderOffset::Loop4::LDWELL
        }
    };

    for (int i = 0; i < 4; ++i)
    {
        writeUInt32(
            d,
            loopOffsets[i][0],
            s.loops[i].position
        );

        writeFixed32_16(
            d,
            loopOffsets[i][1],
            s.loops[i].length
        );

        writeUInt16(
            d,
            loopOffsets[i][2],
            s.loops[i].dwell
        );

        DBG(
            "ENCODE LOOP "
            + juce::String(i + 1)
            + " POSITION="
            + juce::String((juce::int64)s.loops[i].position)
            + " LENGTH="
            + juce::String(s.loops[i].length, 3)
            + " DWELL="
            + juce::String(s.loops[i].dwell)
        );
    }

    DBG(
        "ENCODE SAMPLE RATE = "
        + juce::String(s.sampleRate)
    );

    writeUInt16(
        d,
        SampleHeaderOffset::SSRATE,
        static_cast<uint16_t>(s.sampleRate)
    );



    d[SampleHeaderOffset::SSPARE] =
        s.spare;

    d[SampleHeaderOffset::SWCOMM] =
        s.waveComment;

    d[SampleHeaderOffset::SSPAIR] =
        static_cast<uint8_t>(s.stereoPartner & 0xFF);

    d[SampleHeaderOffset::SSPAIR + 1] =
        static_cast<uint8_t>((s.stereoPartner >> 8) & 0xFF);

    d[SampleHeaderOffset::SSRATE] =
        static_cast<uint8_t>(s.sampleRate & 0xFF);

    d[SampleHeaderOffset::SSRATE + 1] =
        static_cast<uint8_t>((s.sampleRate >> 8) & 0xFF);

    // ==============================
// Sample Rate
// ==============================

    DBG(
        "ENCODE SAMPLE RATE = "
        + juce::String(s.sampleRate)
    );

    writeUInt16(
        d,
        SampleHeaderOffset::SSRATE,
        static_cast<uint16_t>(s.sampleRate)
    );




    return d;
}

std::vector<uint8_t> SampleHeaderEncoder::encodeNibbleData(
    const std::vector<uint8_t>& decoded)
{
    std::vector<uint8_t> encoded;

    encoded.reserve(decoded.size() * 2);

    for (uint8_t value : decoded)
    {
        const uint8_t low =
            value & 0x0F;

        const uint8_t high =
            (value >> 4) & 0x0F;

        encoded.push_back(low);
        encoded.push_back(high);
    }

    return encoded;
}

std::vector<uint8_t>
SampleHeaderEncoder::makeSysEx(
    const SampleHeader& header)
{
    auto decoded =
        encode(header);

    auto encodedData =
        encodeNibbleData(decoded);

    std::vector<uint8_t> data;

    // Akai SysEx payload
    data.push_back(0x47); // Akai
    data.push_back(0x00); // channel
    data.push_back(0x2C); // Sample Header
    data.push_back(0x48); // S1000

    // Sample number
    data.push_back(
        static_cast<uint8_t>(header.id & 0x7F)
    );

    data.push_back(
        static_cast<uint8_t>((header.id >> 7) & 0x7F)
    );

    // Reserved
    data.push_back(0x00);

    // Offset = 0
    data.push_back(0x00);
    data.push_back(0x00);

    // Number of bytes = 141
    data.push_back(0x0D);
    data.push_back(0x01);

    // Actual nibble encoded data
    data.insert(
        data.end(),
        encodedData.begin(),
        encodedData.end()
    );

    return data;
}