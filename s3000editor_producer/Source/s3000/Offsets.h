#pragma once

#include <cstddef>

namespace ProgramOffset
{
    namespace General
    {
        constexpr size_t Number = 14;
        constexpr size_t MidiChannel = 15;
        constexpr size_t Polyphony = 16;
        constexpr size_t Priority = 18;
    }

    namespace Output
    {
        constexpr size_t Output = 22;
        constexpr size_t Stereo = 23;
        constexpr size_t Pan = 24;
        constexpr size_t Loudness = 25;
    }

    namespace LFO
    {
        constexpr size_t LFO2Rate = 29;
        constexpr size_t LFO2Depth = 30;
        constexpr size_t LFO1Rate = 33;
        constexpr size_t LFO1Depth = 34;
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

        constexpr std::size_t SNAME = 0;

        constexpr std::size_t LOVEL = 12;
        constexpr std::size_t HIVEL = 13;

        constexpr std::size_t TUNE = 14;
        constexpr std::size_t LOUDNESS = 15;
        constexpr std::size_t PAN = 16;
        constexpr std::size_t PLAYMODE = 17;

        constexpr std::size_t STRIDE = 24;
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