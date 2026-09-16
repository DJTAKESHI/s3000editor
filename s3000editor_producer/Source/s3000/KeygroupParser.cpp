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

    Keygroup k{};
    if (d.size() < 192)
    {
        DBG("Keygroup data too small");
        return k;
    }

    k.rawData = d;



    // ===== Internal =====
    k.id =
        d[KGH::Common::KGIDENT];

    k.nextAddress =
        d[KGH::Common::NXTKG]
        |
        (d[KGH::Common::NXTKG + 1] << 8);




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

    k.filter.velocityToFreq =
        static_cast<int8_t>(
            d[KGH::Filter::V_FREQ]
            );

    k.filter.pressureToFreq =
        static_cast<int8_t>(
            d[KGH::Filter::P_FREQ]
            );

    k.filter.envelopeToFreq =
        static_cast<int8_t>(
            d[KGH::Filter::E_FREQ]
            );

    k.filter.resonance =
        d[KGH::Filter::FILQ];


    // ===== Velocity Control =====
    k.velocity.vEnv2 =
        static_cast<int8_t>(
            d[KGH::Velocity::V_ENV2]
            );





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


    k.env2.r1 = d[KGH::Env2::R1];
    k.env2.l1 = d[KGH::Env2::L1];

    k.env2.r2 = d[KGH::Env2::R2];
    k.env2.l2 = d[KGH::Env2::L2];

    k.env2.r3 = d[KGH::Env2::R3];
    k.env2.l3 = d[KGH::Env2::L3];

    k.env2.r4 = d[KGH::Env2::R4];
    k.env2.l4 = d[KGH::Env2::L4];


    k.env2.velAttack =
        static_cast<int8_t>(
            d[KGH::Velocity::V_ATT2]
            );

    k.env2.velRelease =
        static_cast<int8_t>(
            d[KGH::Velocity::V_REL2]
            );

    k.env2.noteOffRelease =
        static_cast<int8_t>(
            d[KGH::Velocity::O_REL2]
            );

    k.env2.keyTracking =
        static_cast<int8_t>(
            d[KGH::Velocity::K_DAR2]
            );

    k.velocity.ePtch =
        static_cast<int8_t>(
            d[KGH::Velocity::E_PTCH]
            );


    k.modVPitch =
        static_cast<int8_t>(
            d[KeygroupHeaderOffset::Mod::ModVPitch]
            );


    k.velocity.vxFade =
        d[KGH::Velocity::VXFADE];

    k.velocity.vZones =
        d[KGH::Velocity::VZONES];

    k.velocity.lkxf =
        d[KGH::Velocity::LKXF];

    k.velocity.rkxf =
        d[KGH::Velocity::RKXF];



    // ===== Keygroup Data =====


    for (int i = 0; i < 4; ++i)
    {
        if (KGF::Zone::SNAME[i] + 12 > d.size())
        {
            DBG("No more zones");
            break;
        }

        DBG(
            "ABOUT TO CALL parseZone "
            + juce::String(i)
        );

        k.zones[i] =
            parseZone(
                d,
                i,
                residentSamples
            );

        // CP1〜CP4
        // offset 132〜135
        const std::size_t constantPitchOffset =
            132 + static_cast<std::size_t>(i);

        if (constantPitchOffset < d.size())
        {
            k.zones[i].constantPitch =
                d[constantPitchOffset] != 0;
        }

        DBG(
            "ZONE "
            + juce::String(i + 1)
            + " PITCH MODE = "
            + juce::String(
                k.zones[i].constantPitch
                ? "CONST"
                : "TRACK"
            )
        );
    }

    return k;

}


VelocityZone KeygroupParser::parseZone(
    const std::vector<uint8_t>& d,
    int zoneIndex,
    const std::map<int, juce::String>& residentSamples
)
{

    VelocityZone z{};

    for (int j = 0; j < 32; j++)
    {
        DBG(
            juce::String(j)
            + " : "
            + juce::String::toHexString(
                d[KGF::Zone::SNAME[zoneIndex] + j]
            )
        );
    }

    // ===== Sample Name =====
    for (int i = 0; i < 12; i++)
    {
        z.sampleName +=
            decodePlistChar(
                d[KGF::Zone::SNAME[zoneIndex] + i]
            );
    }




    DBG("ZONE SAMPLE NAME = "
        + juce::String(z.sampleName));

    for (auto& sample : residentSamples)
    {
        auto residentName = sample.second.trim();
        auto zoneName = z.sampleName.trim();



        if (residentName == zoneName)
        {
            z.sampleId = sample.first;

            DBG(
                "MATCH SAMPLE ID = "
                + juce::String(z.sampleId)
            );

            break;
        }
    }




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
        d[KGF::Zone::LOVEL[zoneIndex]];

    z.highVel =
        d[KGF::Zone::HIVEL[zoneIndex]];


    z.fineTuneRaw =
        static_cast<int8_t>(
            d[KGF::Zone::VTUNO[zoneIndex]]
            );

    z.semitone =
        static_cast<int8_t>(
            d[KGF::Zone::VTUNO[zoneIndex] + 1]
            );

    z.fineTuneRaw =
        static_cast<int8_t>(
            d[KGF::Zone::VTUNO[zoneIndex]]
            );

    z.semitone =
        static_cast<int8_t>(
            d[KGF::Zone::VTUNO[zoneIndex] + 1]
            );

    z.filterFreq =
        static_cast<int8_t>(
            d[KGF::Zone::VFREQ[zoneIndex]]
            );

    z.lowVelXFade =
        d[KGF::Zone::LVXF[zoneIndex]];

    z.highVelXFade =
        d[KGF::Zone::HVXF[zoneIndex]];

    z.loudness =
        static_cast<int8_t>(
            d[KGF::Zone::VLOUD[zoneIndex]]
            );


    z.pan =
        static_cast<int8_t>(
            d[KGF::Zone::VPANO[zoneIndex]]
            );


    z.playMode =
        static_cast<PlayMode>(
            d[KGF::Zone::ZPLAY[zoneIndex]]
            );


    


    return z;
}






