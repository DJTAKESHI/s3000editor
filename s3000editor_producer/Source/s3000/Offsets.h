#pragma once

#include <cstddef>

namespace ProgramOffset
{
    namespace General
    {
        constexpr std::size_t Number = 15; // PRGNUM
        constexpr std::size_t MidiChannel = 16; // PMCHAN
        constexpr std::size_t Polyphony = 17; // POLYPH
        constexpr std::size_t Priority = 18; // PRIORT

        constexpr std::size_t PlayLow = 19; // PLAYLO
        constexpr std::size_t PlayHigh = 20; // PLAYHI
    }

    namespace Output
    {
        constexpr size_t Output = 22;
        constexpr size_t Stereo = 23;
        constexpr size_t Pan = 24;
        constexpr size_t Loudness = 25;
        constexpr std::size_t VelocityLoudness = 26;
    }

    namespace LFO
    {
        constexpr std::size_t LFO2Rate = 29;
        constexpr std::size_t LFO2Depth = 30;
        constexpr std::size_t LFO2Delay = 31;

        constexpr std::size_t LFO1Rate = 33;
        constexpr std::size_t LFO1Depth = 34;
        constexpr std::size_t LFO1Delay = 35;

        constexpr std::size_t ModWheelDepth = 36;
        constexpr std::size_t PressureDepth = 37;
        constexpr std::size_t VelocityDepth = 38;

        constexpr std::size_t LFO1Wave = 97;
        constexpr std::size_t LFO2Wave = 98;
    }

    namespace Pitch
    {

        // ===== Pitch =====
        constexpr std::size_t BendUp = 39;
        constexpr std::size_t PressurePitch = 40;

        constexpr std::size_t BendDown = 73;
        constexpr std::size_t BendMode = 74;
        constexpr std::size_t Transpose = 75;
        constexpr std::size_t ProgramTune = 65;   // PTUNO (2 bytes)
    }



    namespace Keygroups
    {
        constexpr std::size_t KeygroupCrossfade = 41;
        constexpr std::size_t NumKeygroups = 42;
        constexpr std::size_t TempProgramNumber = 43;
    }

    namespace Temperament
    {
        constexpr std::size_t Temperament = 44;      // 12 bytes
    }

    namespace Voice
    {
        constexpr std::size_t Desync = 59;
        constexpr std::size_t VoiceAssign = 61;
        constexpr std::size_t VoiceOutput = 70;
        constexpr std::size_t Legato = 72;

    }

    namespace SoftPedal
    {
        constexpr std::size_t SoftLoudness = 62;
        constexpr std::size_t SoftAttack = 63;
        constexpr std::size_t SoftFilter = 64;

    }

    namespace Mod
    {
        constexpr std::size_t ModSPan1 = 76;
        constexpr std::size_t ModSPan2 = 77;
        constexpr std::size_t ModSPan3 = 78;

        constexpr std::size_t ModSAmp1 = 79;
        constexpr std::size_t ModSAmp2 = 80;

        constexpr std::size_t ModSLFO1Rate = 81;
        constexpr std::size_t ModSLFO1Depth = 82;
        constexpr std::size_t ModSLFO1Delay = 83;

        constexpr std::size_t ModSFilter1 = 84;
        constexpr std::size_t ModSFilter2 = 85;
        constexpr std::size_t ModSFilter3 = 86;

        constexpr std::size_t ModSPitch = 87;
        constexpr std::size_t ModSAmp3 = 88;

        constexpr std::size_t ModVPan1 = 89;
        constexpr std::size_t ModVPan2 = 90;
        constexpr std::size_t ModVPan3 = 91;

        constexpr std::size_t ModVAmp1 = 92;
        constexpr std::size_t ModVAmp2 = 93;

        constexpr std::size_t ModVLFO1Rate = 94;
        constexpr std::size_t ModVLFO1Depth = 95;
        constexpr std::size_t ModVLFO1Delay = 96;

    }
}


namespace KeygroupOffset
{
    // ===== Common =====
    namespace Common
    {
        constexpr std::size_t LONOTE = 3;
        constexpr std::size_t HINOTE = 4;
        constexpr std::size_t TUNE = 5;
    }

    // ===== Filter =====
    namespace Filter1
    {
        constexpr std::size_t FREQ = 7;
        constexpr std::size_t KEYFOLLOW = 8;
    }

    // ===== Envelope1 =====
    namespace Env1
    {
        constexpr std::size_t ATTACK = 12;
        constexpr std::size_t DECAY = 13;
        constexpr std::size_t SUSTAIN = 14;
        constexpr std::size_t RELEASE = 15;
    }

    // ===== Envelope2 =====
    namespace Env2
    {
        constexpr std::size_t ATTACK = 20;
        constexpr std::size_t DECAY = 21;
        constexpr std::size_t SUSTAIN = 22;
        constexpr std::size_t RELEASE = 23;
    }

    // ===== Velocity Zones =====
    namespace Zone
    {
        constexpr std::size_t BASE = 34;
        constexpr std::size_t STRIDE = 24;

        constexpr std::size_t SNAME = 0;

        constexpr std::size_t LOVEL = 12;
        constexpr std::size_t HIVEL = 13;

        constexpr std::size_t TUNE = 14;
        constexpr std::size_t LOUDNESS = 15;
        constexpr std::size_t PAN = 16;
        constexpr std::size_t PLAYMODE = 17;

    }

    // Zone2〜4はオフセット+差分で管理するのが基本
    //constexpr std::size_t ZONE_STRIDE = 24;

    // ===== Playback =====
    namespace Playback
    {
        constexpr std::size_t KBEAT = 130;
        constexpr std::size_t AHOLD = 131;
    }

    // ===== ModMatrix =====
    namespace Mod
    {
        constexpr std::size_t MOD_BASE = 151;
    }
}

//namespace KeygroupFullOffset
//{
//    // ===== Velocity Zones =====
//    namespace Zone
//    {
//        constexpr std::size_t BASE = 64;
//
//        constexpr std::size_t SNAME = 0;
//
//        constexpr std::size_t LOVEL = 12;
//        constexpr std::size_t HIVEL = 13;
//
//        constexpr std::size_t TUNE = 14;
//        constexpr std::size_t LOUDNESS = 15;
//        constexpr std::size_t PAN = 16;
//        constexpr std::size_t PLAYMODE = 17;
//
//        constexpr std::size_t STRIDE = 24;
//    }
//
//
//    // ===== Playback =====
//    namespace Playback
//    {
//        constexpr std::size_t KBEAT = 130;
//        constexpr std::size_t AHOLD = 131;
//    }
//
//
//    // ===== ModMatrix =====
//    namespace Mod
//    {
//        constexpr std::size_t MOD_BASE = 151;
//    }
//};

namespace KeygroupHeaderOffset
{
    namespace Common
    {
        constexpr std::size_t KGIDENT = 0;
        constexpr std::size_t NXTKG = 1;

        constexpr std::size_t LONOTE = 3;
        constexpr std::size_t HINOTE = 4;
        constexpr std::size_t KGTUNO = 5;
    }


    namespace Filter
    {
        constexpr std::size_t FILFRQ = 7;
        constexpr std::size_t K_FREQ = 8;
        constexpr std::size_t V_FREQ = 9;
        constexpr std::size_t P_FREQ = 10;
        constexpr std::size_t E_FREQ = 11;
    }


    namespace Env1
    {
        constexpr std::size_t ATTACK = 12;
        constexpr std::size_t DECAY = 13;
        constexpr std::size_t SUSTAIN = 14;
        constexpr std::size_t RELEASE = 15;
    }


    namespace Env2
    {
        constexpr std::size_t R1 = 20;
        constexpr std::size_t R3 = 21;
        constexpr std::size_t L3 = 22;
        constexpr std::size_t R4 = 23;

        constexpr std::size_t L1 = 156;
        constexpr std::size_t R2 = 157;
        constexpr std::size_t L2 = 158;
        constexpr std::size_t L4 = 159;

        // compatibility aliases
        constexpr std::size_t ATTACK = R1;
        constexpr std::size_t DECAY = R3;
        constexpr std::size_t SUSTAIN = L3;
        constexpr std::size_t RELEASE = R4;
    }


    namespace Velocity
    {
        constexpr std::size_t V_ATT2 = 24;
        constexpr std::size_t V_REL2 = 25;
        constexpr std::size_t O_REL2 = 26;
        constexpr std::size_t K_DAR2 = 27;

        constexpr std::size_t V_ENV2 = 28;

        constexpr std::size_t E_PTCH = 29;
        constexpr std::size_t VXFADE = 30;
        constexpr std::size_t VZONES = 31;

        constexpr std::size_t LKXF = 32;
        constexpr std::size_t RKXF = 33;
    }
}


// =================================
// Keygroup Full 132 byte
// =================================
namespace KeygroupFullOffset
{
    namespace Zone
    {

        constexpr size_t SNAME[4] =
        {
            34,
            58,
            82,
            106
        };


        constexpr size_t LOVEL[4] =
        {
            46,
            70,
            94,
            118
        };


        constexpr size_t HIVEL[4] =
        {
            47,
            71,
            95,
            119
        };


        constexpr size_t VTUNO[4] =
        {
            48,
            72,
            96,
            120
        };


        constexpr size_t VLOUD[4] =
        {
            50,
            74,
            98,
            122
        };


        constexpr size_t VPANO[4] =
        {
            52,
            76,
            100,
            124
        };


        constexpr size_t ZPLAY[4] =
        {
            53,
            77,
            101,
            125
        };

        constexpr std::array<std::size_t, 4> VFREQ =
        {
            51,   // Zone 1
            75,   // Zone 2
            99,   // Zone 3
            123   // Zone 4
        };

        constexpr std::array<std::size_t, 4> LVXF =
        {
            54,
            78,
            102,
            126
        };

        constexpr std::array<std::size_t, 4> HVXF =
        {
            55,
            79,
            103,
            127
        };



    }
}


namespace SampleHeaderOffset
{
    constexpr size_t SHIDENT = 0;
    constexpr size_t SBANDW = 1;
    constexpr size_t SPITCH = 2;

    constexpr size_t SHNAME = 3;

    constexpr size_t SSRVLD = 15;

    constexpr size_t SLOOPS = 16;
    constexpr size_t SALOOP = 17;
    constexpr size_t SHLOOP = 18;

    constexpr size_t SPTYPE = 19;

    constexpr size_t STUNO = 20;

    constexpr size_t SLOCAT = 22;
    constexpr size_t SLNGTH = 26;
    constexpr size_t SSTART = 30;
    constexpr size_t SMPEND = 34;


    namespace Loop1
    {
        constexpr size_t LOOPAT = 38;
        constexpr size_t LLNGTH = 42;
        constexpr size_t LDWELL = 48;
    }


    namespace Loop2
    {
        constexpr size_t LOOPAT = 50;
        constexpr size_t LLNGTH = 54;
        constexpr size_t LDWELL = 60;
    }


    namespace Loop3
    {
        constexpr size_t LOOPAT = 62;
        constexpr size_t LLNGTH = 66;
        constexpr size_t LDWELL = 72;
    }

    namespace Loop4
    {
        constexpr size_t LOOPAT = 74;  // 74-77
        constexpr size_t LLNGTH = 78;  // 78-83
        constexpr size_t LDWELL = 84;  // 84-85
    }

    constexpr size_t SLXY1 = 86;
    constexpr size_t SLXY2 = 98;
    constexpr size_t SLXY3 = 110;
    constexpr size_t SLXY4 = 122;

    constexpr size_t SSPARE = 134;
    constexpr size_t SWCOMM = 135;
    constexpr size_t SSPAIR = 136;
    constexpr size_t SSRATE = 138;
    constexpr size_t SHLTO = 140;

}

namespace SampleHeaderFormat
{
    constexpr int Size = 141;
}