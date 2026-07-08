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


struct Filter1
{
    int freq = 0;
    int keyFollow = 0;
    int resonance = 0;
};
    
struct PlaybackControl
{
    int kbeat = 0;
    uint8_t ahold = 0;

    uint8_t constantPitch = 0;

    int keygroupXfade = 0;

};

enum class PlayMode : uint8_t
{
    AsSample = 0,
    LoopInRelease = 1,
    LoopTilRelease = 2,
    NoLoop = 3,
    PlayToEnd = 4
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
    Off = 0,
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
    None = 0,
    Pan,
    Amp,
    FilterFreq,
    Pitch,
    LFO1Rate,
    LFO1Depth,
    LFO1Delay

};

struct ModContext
{
    float lfo1 = 0.0f;
    float lfo2 = 0.0f;
    float env1 = 0.0f;
    float env2 = 0.0f;

    float velocity = 0.0f;
    float aftertouch = 0.0f;
    float modWheel = 0.0f;

    float keytrack = 0.0f;
    float pitchBend = 0.0f;

    float get(ModSource s) const;
};

struct ModSlot
{
    ModSource source = ModSource::Off;
    ModDestination dest = ModDestination::None;

    int amount = 0;

    float normalized() const
    {
        return amount / 50.0f;
    }

};

struct ModMatrix
{
    std::array<ModSlot, 6> slots;

    float evaluate(const ModContext& ctx,
        ModDestination dest) const;
};

struct Keygroup
{

    int lowNote;
    int highNote;

    int tune;

    Filter1 filter1;


    Envelope env1;
    Envelope env2;



    std::array<VelocityZone, 4> zones;

    PlaybackControl playback;

    ModMatrix mod;



};

struct Program
{
    int programNumber;
    std::string name;
    int midiChannel;
    int polyphony;

    std::vector<Keygroup> keygroups;
};