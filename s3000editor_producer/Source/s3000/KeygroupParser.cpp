#include "KeygroupParser.h"
#include "Offsets.h"

using K = KeygroupOffset;

Keygroup KeygroupParser::parse(const std::vector<uint8_t>& d)
{
    Keygroup k{};

    // ===== Common =====
    k.lowNote = d[K::Common::LONOTE];
    k.highNote = d[K::Common::HINOTE];
    k.tune = (int8_t)d[K::Common::TUNE];

    // ===== Filter =====
    k.filter1.freq =
        d[K::Filter1::FREQ];

    k.filter1.keyFollow =
        d[K::Filter1::KEYFOLLOW];

    // ===== Env1 =====
    k.env1.attack =
        d[K::Env1::ATTACK];

    k.env1.decay =
        d[K::Env1::DECAY];

    k.env1.sustain =
        d[K::Env1::SUSTAIN];

    k.env1.release =
        d[K::Env1::RELEASE];

    // ===== Playback =====
    k.playback.kbeat =
        d[K::Playback::KBEAT];

    k.playback.ahold =
        d[K::Playback::AHOLD];

    return k;

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



