#include "KeygroupParser.h"
#include "Offsets.h"

namespace KG = KeygroupOffset;

Keygroup KeygroupParser::parse(const std::vector<uint8_t>& d)
{
    Keygroup k{};

    if (d.size() < 132)
    {
        return k;
    }

    // ===== Common =====
    k.lowNote = d[KG::Common::LONOTE];
    k.highNote = d[KG::Common::HINOTE];
    k.tune = (int8_t)d[KG::Common::TUNE];

    // ===== Filter =====
    k.filter.freq =
        d[KG::Filter1::FREQ];

    k.filter.keyFollow =
        d[KG::Filter1::KEYFOLLOW];

    // ===== Env1 =====
    k.env1.attack =
        d[KG::Env1::ATTACK];

    k.env1.decay =
        d[KG::Env1::DECAY];

    k.env1.sustain =
        d[KG::Env1::SUSTAIN];

    k.env1.release =
        d[KG::Env1::RELEASE];

    // ===== Playback =====
    k.playback.kbeat =
        d[KG::Playback::KBEAT];

    k.playback.ahold =
        d[KG::Playback::AHOLD];

    //==========Velocity Zones=======
    for (int i = 0; i < 4; ++i)
    {
        size_t base = 
            KG::Zone::BASE + i * KG::Zone::STRIDE;

        //auto& zone = k.zones[i];

        //zone.lowVel = d[base + KG::Zone::LOVEL];
        //zone.highVel = d[base + KG::Zone::HIVEL];

        k.zones[i] = parseZone(d, base);
    }
    return k;

}

VelocityZone KeygroupParser::parseZone(
    const std::vector<uint8_t>& d,
    size_t base
)
{
    VelocityZone z{};

    

    for (int i = 0; i < 12; ++i)
    {
        char c = static_cast<char>(
            d[base + KG::Zone::SNAME + i]
        );

        if (c == 0)
        {
            break;
        }

        z.sampleName += c;
    }

    z.lowVel = d[base + KG::Zone::LOVEL];
    z.highVel = d[base + KG::Zone::HIVEL];

    z.tune = static_cast<int8_t>(d[base + KG::Zone::TUNE]);
    z.loudness = static_cast<int8_t>(d[base + KG::Zone::LOUDNESS]);
    z.pan = static_cast<int8_t>(d[base + KG::Zone::PAN]);
    z.playMode = static_cast<PlayMode>(d[base + KG::Zone::PLAYMODE]);



    return z;

}

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



