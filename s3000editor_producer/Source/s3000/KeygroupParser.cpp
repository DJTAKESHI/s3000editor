#include <JuceHeader.h>

#include "KeygroupParser.h"
#include "Offsets.h"
#include "SysExUtils.h"

namespace KGH = KeygroupHeaderOffset;
namespace KGF = KeygroupFullOffset;

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


Keygroup KeygroupParser::parse(
    const std::vector<uint8_t>& d,
    const std::map<int, juce::String>& residentSamples)
{
    DBG("=== RAW KEYGROUP FIRST 64 ===");

    for (int i = 0; i < 64; i++)
    {
        DBG(
            juce::String(i)
            + " : 0x"
            + juce::String::toHexString(d[i])
        );
    }

    DBG("parse size = " + juce::String((int)d.size()));
    Keygroup k{};
    if (d.size() < 64)
    {
        DBG("Keygroup data too small");
        return k;
    }

    // ===== Internal =====
    k.id =
        d[KGH::Common::KGIDENT];

    k.nextAddress =
        d[KGH::Common::NXTKG]
        |
        (d[KGH::Common::NXTKG + 1] << 8);



    //DBG("Keygroup parse size = "
    //    + juce::String((int)d.size()));

    //DBG("LONOTE offset = "
    //    + juce::String(KG::Common::LONOTE));

    //DBG("HINOTE offset = "
    //    + juce::String(KG::Common::HINOTE));

    //DBG("TUNE offset = "
    //    + juce::String(KG::Common::TUNE));

    //DBG("FREQ offset = "
    //    + juce::String(KG::Filter1::FREQ));



    //if (d.size() < 10)
    //{
    //    return k;
    //}


    DBG("LONOTE index = "
        + juce::String(KGH::Common::LONOTE));

    DBG("HINOTE index = "
        + juce::String(KGH::Common::HINOTE));


    // ===== Common =====
    k.lowNote =
        d[KGH::Common::LONOTE];

    k.highNote =
        d[KGH::Common::HINOTE];

    k.tune =
        (int8_t)d[KGH::Common::KGTUNO];


    // ===== Filter =====
    k.filter.freq =
        d[KGH::Filter::FILFRQ];

    k.filter.keyFollow =
        d[KGH::Filter::K_FREQ];


    // ===== Env1 =====
    k.env1.attack =
        d[KGH::Env1::ATTACK];

    k.env1.decay =
        d[KGH::Env1::DECAY];

    k.env1.sustain =
        d[KGH::Env1::SUSTAIN];

    k.env1.release =
        d[KGH::Env1::RELEASE];


    // ===== Env2 =====
    k.env2.attack =
        d[KGH::Env2::ATTACK];

    k.env2.decay =
        d[KGH::Env2::DECAY];

    k.env2.sustain =
        d[KGH::Env2::SUSTAIN];

    k.env2.release =
        d[KGH::Env2::RELEASE];

    // ===== Velocity Control =====

    k.velocity.vAtt2 =
        (int8_t)d[KGH::Velocity::V_ATT2];

    k.velocity.vRel2 =
        (int8_t)d[KGH::Velocity::V_REL2];

    k.velocity.oRel2 =
        (int8_t)d[KGH::Velocity::O_REL2];

    k.velocity.kDar2 =
        (int8_t)d[KGH::Velocity::K_DAR2];


    k.velocity.vEnv2 =
        (int8_t)d[KGH::Velocity::V_ENV2];


    k.velocity.ePtch =
        d[KGH::Velocity::E_PTCH];

    k.velocity.vxFade =
        d[KGH::Velocity::VXFADE];

    k.velocity.vZones =
        d[KGH::Velocity::VZONES];


    k.velocity.lkxf =
        d[KGH::Velocity::LKXF];

    k.velocity.rkxf =
        d[KGH::Velocity::RKXF];

    // ===== Keygroup Data =====

    if (d.size() >= 192)
    {
        DBG("=== KEYGROUP DATA RAW ===");

        for (int i = 0; i < 192; i++)
        {
            DBG(
                juce::String(i)
                + " : 0x"
                + juce::String::toHexString(d[i])
            );
        }
    }


    // ===== Zones =====

    if (d.size() >= KGF::Zone1::BASE + KGF::Zone1::STRIDE)
    {
        for (int i = 0; i < 4; i++)
        {
            size_t base =
                KGF::Zone1::BASE
                + i * KGF::Zone1::STRIDE;

            if (base + KGF::Zone1::STRIDE > d.size())
            {
                DBG("No more zones");
                break;
            }

            k.zones[i] =
                parseZone(d, 
                            base,
                            residentSamples);
        }
    }
    DBG("=== KEYGROUP HEADER ===");

    DBG("LOW NOTE = "
        + juce::String(k.lowNote));

    DBG("HIGH NOTE = "
        + juce::String(k.highNote));

    DBG("FILTER FREQ = "
        + juce::String(k.filter.freq));

    DBG("ENV1 ATTACK = "
        + juce::String(k.env1.attack));

    DBG("ENV1 DECAY = "
        + juce::String(k.env1.decay));

    DBG("ENV2 ATTACK = "
        + juce::String(k.env2.attack));

    DBG("V_ENV2 = "
        + juce::String(k.velocity.vEnv2));

    DBG("VXFADE = "
        + juce::String(k.velocity.vxFade));

    DBG("VZONES = "
        + juce::String(k.velocity.vZones));

    DBG("LKXF = "
        + juce::String(k.velocity.lkxf));

    DBG("RKXF = "
        + juce::String(k.velocity.rkxf));



    return k;

}

//KeygroupHeader KeygroupHeaderParser::parse(
//    const std::vector<uint8_t>& d)
//{
//    KeygroupHeader k{};
//
//    k.id = d[0];
//
//    k.nextBlock =
//        d[1] |
//        (d[2] << 7);
//
//    k.lowNote = d[3];
//
//    k.highNote = d[4];
//
//    k.tune =
//        d[5] |
//        (d[6] << 8);
//
//    k.filterFreq = d[7];
//
//    k.keyFollow = d[8];
//
//    return k;
//}

VelocityZone KeygroupParser::parseZone(
    const std::vector<uint8_t>& d,
    size_t base,
    const std::map<int, juce::String>& residentSamples
    //const std::map<int, SampleHeader>& samples
)
{
    VelocityZone z{};

    // ===== Sample Name =====
    for (int i = 0; i < 12; i++)
    {
        z.sampleName +=
            decodePlistChar(
                d[base + KGF::Zone1::SNAME + i]
            );
    }


    DBG("ZONE SAMPLE NAME = "
        + juce::String(z.sampleName));

    for (auto& sample : residentSamples)
    {
        if (sample.second.trim() == z.sampleName)
        {
            z.sampleId = sample.first;

            DBG("MATCH SAMPLE ID = "
                + juce::String(z.sampleId));

            break;
        }
    }

    DBG("ZONE SAMPLE NAME = "
        + juce::String(z.sampleName));


    for (auto& sample : residentSamples)
    {
        DBG(
            juce::String(sample.first)
            + " = "
            + sample.second
        );
    }


    // ===== Velocity =====

    z.lowVel =
        d[base + KGF::Zone1::LOVEL];

    z.highVel =
        d[base + KGF::Zone1::HIVEL];


    z.tune =
        static_cast<int8_t>(
            d[base + KGF::Zone1::VTUNO]
            );


    z.loudness =
        static_cast<int8_t>(
            d[base + KGF::Zone1::VLOUD]
            );


    z.pan =
        static_cast<int8_t>(
            d[base + KGF::Zone1::VPANO]
            );


    z.playMode =
        static_cast<PlayMode>(
            d[base + KGF::Zone1::ZPLAY]
            );

    //auto it = samples.find(z.sampleId);

    //if (it != samples.end())
    //{
    //    z.sampleName = it->second.name;
    //}

    return z;
}


//void KeygroupParser::resolveSampleNames(
//    Keygroup& k,
//    const std::vector<std::string>& sampleNames)
//{
//    for (auto& zone : k.zones)
//    {
//        if (zone.sampleId < sampleNames.size())
//        {
//            zone.sampleName =
//                sampleNames[zone.sampleId];
//        }
//    }
//}

//void KeygroupParser::dumpKeygroup(const std::vector<uint8_t>& decoded)
//{
//    DBG("==== KEYGROUP ====");
//    DBG("Size = " + juce::String((int)decoded.size()));
//
//    static int dumpIndex = 0;
//
//    juce::File file =
//        juce::File::getSpecialLocation(juce::File::userDesktopDirectory)
//        .getChildFile("keygroup_" + juce::String(dumpIndex++) + ".bin");
//
//    file.replaceWithData(decoded.data(), decoded.size());
//}



