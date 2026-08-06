#pragma once

#include <JuceHeader.h>
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
    // Zone sample reference
    juce::String sampleName;

    int sampleId = -1;   // Œã‚Å‰ðŒˆ‚·‚é


    // Velocity range
    uint8_t lowVel;
    uint8_t highVel;


    // Sample parameters
    int8_t tune;
    int8_t loudness;
    int8_t pan;


    PlayMode playMode;
};

struct Sample
{
    int number = -1;
    std::string name;

    int sampleRate = 0;
    int length = 0;
    int tune = 0;
    int rootKey = 60;
    //int start = 0;
    //int end = 0;

    bool loop = false;
    int loopStart = 0;
    int loopEnd = 0;

};


struct VelocityControl
{
    int8_t vAtt2 = 0;
    int8_t vRel2 = 0;
    int8_t oRel2 = 0;
    int8_t kDar2 = 0;

    int8_t vEnv2 = 0;

    uint8_t ePtch = 0;
    uint8_t vxFade = 0;
    uint8_t vZones = 0;

    uint8_t lkxf = 0;
    uint8_t rkxf = 0;
};


struct Keygroup
{
    // ===== Internal block information =====
    uint8_t id = 0;              // KGIDENT
    uint16_t nextAddress = 0;    // NXTKG@

    // ===== Key range =====
    int lowNote = 0;
    int highNote = 127;
    int tune = 0;

    // ===== Filter =====
    FilterParams filter;

    // ===== Envelopes =====
    Envelope env1;
    Envelope env2;

    // ===== Velocity zones =====
    std::array<VelocityZone, 4> zones;

    // ===== Playback =====
    PlaybackParams playback;

    // ===== Velocity control =====
    VelocityControl velocity;
};

struct KeygroupHeader
{
    int id = 0;
    int nextBlock = 0;
    int lowNote = 0;
    int highNote = 127;
    int tune = 0;
    int filterFreq = 0;
    int filterKeyFollow = 0;
};


struct Program
{
    int programNumber = 0;
    std::string name;

    int midiChannel = 1;
    int polyphony = 0;

    int groups = 0;

    std::vector<Keygroup> keygroups;
};

struct SampleMemory
{
    std::vector<Sample> samples;
};

struct SampleHeader
{
    int id = 0;

    int bandwidth = 0;
    int originalPitch = 0;

    juce::String name;

    int sampleRateValid = 0;

    int numLoops = 0;
    int activeLoop = 0;
    int highestLoop = 0;

    int playType = 0;

    int tune = 0;

    uint32_t location = 0;
    uint32_t length = 0;
    uint32_t start = 0;
    uint32_t end = 0;

    struct Loop
    {
        uint32_t position = 0;
        uint32_t length = 0;
        uint16_t dwell = 0;
    };

    std::array<Loop, 3> loops;
};


struct KeyZone
{
    uint16_t sampleId;
};