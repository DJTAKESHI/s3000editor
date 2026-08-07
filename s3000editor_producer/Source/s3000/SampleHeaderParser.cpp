#include <JuceHeader.h>
#include "SampleHeaderParser.h"
#include "Offsets.h"

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

    if (d.size() < 132)
    {
        DBG("Sample header too small");
        return s;
    }


    // š‚±‚ê‚ÍAKAI‚ÌSample”Ô†
    // Request‚µ‚½”Ô†‚ðŽg‚¤
    s.id = sampleIndex;


    DBG("SAMPLE INDEX = "
        + juce::String(s.id));


    s.bandwidth =
        d[SampleHeaderOffset::SBANDW];


    s.originalPitch =
        d[SampleHeaderOffset::SPITCH];

    DBG("SHNAME OFFSET = "
        + juce::String(SampleHeaderOffset::SHNAME));


    for (int i = 0; i < 20; i++)
    {
        DBG(
            "NAME RAW "
            + juce::String(i)
            + " : 0x"
            + juce::String::toHexString(
                d[SampleHeaderOffset::SHNAME + i]
            )
        );
    }


    for (int i = 0; i < 12; i++)
    {
        s.name += decodePlistChar(
            d[SampleHeaderOffset::SHNAME + i]
        );
    }


    s.sampleRateValid =
        d[SampleHeaderOffset::SSRVLD];


    s.numLoops =
        d[SampleHeaderOffset::SLOOPS];


    s.playType =
        d[SampleHeaderOffset::SPTYPE];


    s.tune =
        d[SampleHeaderOffset::STUNO];


    DBG(
        "Stored Sample "
        + juce::String(s.id)
        + " : "
        + s.name
    );


    return s;
}