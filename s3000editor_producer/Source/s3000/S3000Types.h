#pragma once

#include <array>
#include <string>
#include <vector>


struct Envelope
{
    int attack = 0;
    int decay = 0;
    int sustain = 0;
    int release = 0;

    int velAttack = 0;
    int velRelease = 0;
    int noteOffRelease = 0;
    int keyTracking = 0;
};


struct FilterParams
{
    int freq = 0;
    int keyFollow = 0;
    int resonance = 0;
};


struct PlaybackParams
{
    int kbeat = 0;
    uint8_t ahold = 0;
    uint8_t constantPitch = 0;
    int keygroupXfade = 0;
};


enum class PlayMode : uint8_t
{
    AsSample,
    LoopInRelease,
    LoopTilRelease,
    NoLoop,
    PlayToEnd
};


struct VelocityZone
{
    std::string sampleName;

    int lowVel = 0;
    int highVel = 127;

    int tune = 0;
    int loudness = 0;
    int pan = 0;

    PlayMode playMode = PlayMode::AsSample;
};


enum class ModSource
{
    Off,
    LFO1,
    LFO2,
    ENV1,
    ENV2,
    Velocity,
    Aftertouch,
    ModWheel,
    Keytrack,
    PitchBend
};


enum class ModDestination
{
    None,
    Pan,
    Amp,
    FilterFreq,
    Pitch
};


struct ModSlot
{
    ModSource source = ModSource::Off;
    ModDestination dest = ModDestination::None;

    int amount = 0;
};


struct ModMatrix
{
    std::array<ModSlot, 6> slots;
};


struct Keygroup
{
    int lowNote = 0;
    int highNote = 127;
    int tune = 0;

    FilterParams filter;

    Envelope env1;
    Envelope env2;

    std::array<VelocityZone, 4> zones;

    PlaybackParams playback;

    ModMatrix modulation;
};


struct Program
{
    int programNumber = 0;
    std::string name;

    int midiChannel = 1;
    int polyphony = 0;

    std::vector<Keygroup> keygroups;
};