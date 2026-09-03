#include "KeygroupEncoder.h"
#include "Offsets.h"

namespace KGF = KeygroupFullOffset;

static uint8_t encodePlistChar(
    juce::juce_wchar c)
{
    if (c >= '0' && c <= '9')
        return static_cast<uint8_t>(c - '0');

    if (c == ' ')
        return 10;

    if (c >= 'A' && c <= 'Z')
        return static_cast<uint8_t>(
            11 + (c - 'A')
            );

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

std::vector<uint8_t> KeygroupEncoder::encode(
    const Keygroup& keygroup
)
{



    // ���@�����M��������192 bytes���R�s�[
    auto data = keygroup.rawData;

    if (data.size() < 192)
    {
        DBG("KEYGROUP ENCODE ERROR: rawData too small");
        return {};
    }




    // ===== Keygroup Common =====
    data[KeygroupHeaderOffset::Common::LONOTE] =
        static_cast<uint8_t>(keygroup.lowNote);

    data[KeygroupHeaderOffset::Common::HINOTE] =
        static_cast<uint8_t>(keygroup.highNote);

    data[KeygroupHeaderOffset::Common::KGTUNO] =
        static_cast<uint8_t>(keygroup.tune);

    // ===== Filter =====
    data[KeygroupHeaderOffset::Filter::FILFRQ] =
        static_cast<uint8_t>(
            keygroup.filter.freq
            );

    data[KeygroupHeaderOffset::Filter::K_FREQ] =
        static_cast<uint8_t>(
            keygroup.filter.keyFollow
            );

    data[KeygroupHeaderOffset::Filter::V_FREQ] =
        static_cast<uint8_t>(
            keygroup.filter.velocityToFreq
            );

    data[KeygroupHeaderOffset::Filter::P_FREQ] =
        static_cast<uint8_t>(
            keygroup.filter.pressureToFreq
            );

    data[KeygroupHeaderOffset::Filter::E_FREQ] =
        static_cast<uint8_t>(
            keygroup.filter.envelopeToFreq
            );

    data[KeygroupHeaderOffset::Filter::FILQ] =
        static_cast<uint8_t>(
            juce::jlimit(
                0,
                15,
                keygroup.filter.resonance
            )
            );

    DBG(
        "ENCODED FILQ OFFSET 149 = "
        + juce::String(
            data[
                KeygroupHeaderOffset::Filter::FILQ
            ]
        )
    );

    // ===== ENV1 =====
    data[KeygroupHeaderOffset::Env1::ATTACK] =
        static_cast<uint8_t>(
            keygroup.env1.attack
            );

    data[KeygroupHeaderOffset::Env1::DECAY] =
        static_cast<uint8_t>(keygroup.env1.decay);

    data[KeygroupHeaderOffset::Env1::SUSTAIN] =
        static_cast<uint8_t>(keygroup.env1.sustain);

    data[KeygroupHeaderOffset::Env1::RELEASE] =
        static_cast<uint8_t>(keygroup.env1.release);

    // ===== ENV2 =====
    //data[KeygroupHeaderOffset::Env2::ATTACK] =
    //    static_cast<uint8_t>(keygroup.env2.attack);

    //data[KeygroupHeaderOffset::Env2::DECAY] =
    //    static_cast<uint8_t>(keygroup.env2.decay);

    //data[KeygroupHeaderOffset::Env2::SUSTAIN] =
    //    static_cast<uint8_t>(keygroup.env2.sustain);

    //data[KeygroupHeaderOffset::Env2::RELEASE] =
    //    static_cast<uint8_t>(keygroup.env2.release);

    data[KeygroupHeaderOffset::Env2::R1] =
        static_cast<uint8_t>(keygroup.env2.r1);

    data[KeygroupHeaderOffset::Env2::L1] =
        static_cast<uint8_t>(keygroup.env2.l1);

    data[KeygroupHeaderOffset::Env2::R2] =
        static_cast<uint8_t>(keygroup.env2.r2);

    data[KeygroupHeaderOffset::Env2::L2] =
        static_cast<uint8_t>(keygroup.env2.l2);

    data[KeygroupHeaderOffset::Env2::R3] =
        static_cast<uint8_t>(keygroup.env2.r3);

    data[KeygroupHeaderOffset::Env2::L3] =
        static_cast<uint8_t>(keygroup.env2.l3);

    data[KeygroupHeaderOffset::Env2::R4] =
        static_cast<uint8_t>(keygroup.env2.r4);

    data[KeygroupHeaderOffset::Env2::L4] =
        static_cast<uint8_t>(keygroup.env2.l4);


    data[KeygroupHeaderOffset::Velocity::V_ATT2] =
        static_cast<uint8_t>(
            keygroup.env2.velAttack
            );

    data[KeygroupHeaderOffset::Velocity::V_REL2] =
        static_cast<uint8_t>(
            keygroup.env2.velRelease
            );

    data[KeygroupHeaderOffset::Velocity::O_REL2] =
        static_cast<uint8_t>(
            keygroup.env2.noteOffRelease
            );

    data[KeygroupHeaderOffset::Velocity::K_DAR2] =
        static_cast<uint8_t>(
            keygroup.env2.keyTracking
            );

    data[KeygroupHeaderOffset::Velocity::V_ENV2] =
        static_cast<uint8_t>(
            keygroup.velocity.vEnv2
            );

    data[KeygroupHeaderOffset::Velocity::E_PTCH] =
        static_cast<uint8_t>(
            static_cast<int8_t>(
                keygroup.velocity.ePtch
                )
            );

    data[KeygroupHeaderOffset::Velocity::VXFADE] =
        keygroup.velocity.vxFade;

    data[KeygroupHeaderOffset::Velocity::VZONES] =
        keygroup.velocity.vZones;

    data[KeygroupHeaderOffset::Velocity::LKXF] =
        keygroup.velocity.lkxf;

    data[KeygroupHeaderOffset::Velocity::RKXF] =
        keygroup.velocity.rkxf;

    data[KeygroupHeaderOffset::Mod::ModVPitch] =
        static_cast<uint8_t>(
            static_cast<int8_t>(
                keygroup.modVPitch
                )
            );

    DBG(
        "ENCODE KG MODVPITCH OFFSET154 RAW="
        + juce::String(
            (int)data[
                KeygroupHeaderOffset::Mod::ModVPitch
            ]
        )
    );


    // ===== Velocity Zones =====
    for (int i = 0; i < 4; ++i)
    {
        const auto& zone = keygroup.zones[i];

        // ==============================
        // Sample Name
        // ==============================

        const std::size_t zoneBase =
            KeygroupOffset::Zone::BASE
            + i * KeygroupOffset::Zone::STRIDE;

        juce::String name =
            zone.sampleName;

        // Akai sample name = 12 characters
        while (name.length() < 12)
            name += " ";

        name = name.substring(0, 12);

        for (int j = 0; j < 12; ++j)
        {
            data[
                zoneBase
                    + KeygroupOffset::Zone::SNAME
                    + j
            ] =
                encodePlistChar(name[j]);
        }

        // ==============================
        // Existing parameters
        // ==============================

        data[KGF::Zone::LOVEL[i]] =
            static_cast<uint8_t>(zone.lowVel);

        data[KGF::Zone::HIVEL[i]] =
            static_cast<uint8_t>(zone.highVel);

        data[KGF::Zone::VTUNO[i]] =
            static_cast<uint8_t>(zone.fineTuneRaw);

        data[KGF::Zone::VTUNO[i] + 1] =
            static_cast<uint8_t>(zone.semitone);

        data[KGF::Zone::VLOUD[i]] =
            static_cast<uint8_t>(zone.loudness);

        data[KGF::Zone::VFREQ[i]] =
            static_cast<uint8_t>(zone.filterFreq);

        data[KGF::Zone::VPANO[i]] =
            static_cast<uint8_t>(zone.pan);

        data[KGF::Zone::ZPLAY[i]] =
            static_cast<uint8_t>(zone.playMode);

        // ==============================
// Pitch Tracking
// CP1〜CP4: offset 132〜135
// 0 = TRACK, 1 = CONST
// ==============================

        const std::size_t constantPitchOffset =
            132 + static_cast<std::size_t>(i);

        data[constantPitchOffset] =
            zone.constantPitch ? 1 : 0;


        // internal use�Ȃ̂ŏ��������Ȃ�
        // data[KGF::Zone::LVXF[i]] = zone.lowVelXFade;
        // data[KGF::Zone::HVXF[i]] = zone.highVelXFade;
    }

    return data;
}
