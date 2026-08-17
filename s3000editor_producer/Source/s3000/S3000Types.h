#pragma once

#include <JuceHeader.h>
#include <array>
#include <string>
#include <vector>

//struct Envelope
//{
//    int attack = 0;
//    int decay = 0;
//    int sustain = 0;
//    int release = 0;
//
//    int velAttack = 0;
//    int velRelease = 0;
//    int noteOffRelease = 0;
//    int keyTracking = 0;
//};

struct Envelope1
{
    int attack = 0;
    int decay = 0;
    int sustain = 0;
    int release = 0;
};

struct Envelope2
{
    int r1 = 0;
    int l1 = 0;

    int r2 = 0;
    int l2 = 0;

    int r3 = 0;
    int l3 = 0;

    int r4 = 0;
    int l4 = 0;

    // ENV2 modulation / control
    int velAttack = 0;
    int velRelease = 0;
    int noteOffRelease = 0;
    int keyTracking = 0;
};



struct FilterParams
{
    int freq = 0;
    int keyFollow = 0;

    int velocityToFreq = 0;
    int pressureToFreq = 0;
    int envelopeToFreq = 0;

    int resonance = 0; // Ç‹Çæoffsetñ¢ì¡íËÇ»ÇÁécÇµÇƒOK
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
    AsSample = 0,
    NormalLoop = 1,
    LoopTilRelease = 2,
    NoLoop = 3,
    PlayToEnd = 4
};





struct VelocityZone
{
    juce::String sampleName;
    int sampleId = -1;

    uint8_t lowVel = 0;
    uint8_t highVel = 127;

    int semitone = 0;
    int fineTuneRaw = 0;

    int8_t loudness = 0;

    // Åöí«â¡
    int8_t filterFreq = 0;

    int8_t pan = 0;

    PlayMode playMode;

    // Åöí«â¡
    uint8_t lowVelXFade = 0;
    uint8_t highVelXFade = 0;
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
    Envelope1 env1;
    Envelope2 env2;

    // ===== Velocity zones =====
    std::array<VelocityZone, 4> zones;

    // ===== Playback =====
    PlaybackParams playback;

    // ===== Velocity control =====
    VelocityControl velocity;
    std::vector<uint8_t> rawData;
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


//struct Program
//{
//    int programNumber = 0;
//    std::string name;
//
//    int midiChannel = 1;
//    int polyphony = 0;
//
//    int groups = 0;
//
//    std::vector<Keygroup> keygroups;
//};

struct Program
{
    int programNumber = 0;
    std::string name;

    int midiChannel = 0;
    int polyphony = 0;
    int priority = 1;

    int playLow = 21;
    int playHigh = 127;

    int output = 0;
    int stereoLevel = 99;
    int pan = 0;
    int loudness = 99;

    int lfo1Rate = 0;
    int lfo1Depth = 0;
    int lfo1Delay = 0;

    int lfo2Rate = 0;
    int lfo2Depth = 0;
    int lfo2Delay = 0;

    int modWheelDepth = 0;
    int pressureDepth = 0;
    int velocityDepth = 0;

    int velocityLoudness = 0;

    int bendUp = 0;
    int pressurePitch = 0;

    bool keygroupCrossfade = false;

    int groups = 0; // read-only

    double tune = 0.0;

    int individualOutputLevel = 0; 
    bool legato = false;
    int bendDown = 0;
    int bendMode = 0;
    int transpose = 0;

    bool lfo1Desync = false;
    int voiceAssign = 0;

    int softLoudness = 0;
    int softAttack = 0;
    int softFilter = 0;

    int modSPan1 = 0;
    int modSPan2 = 0;
    int modSPan3 = 0;

    int modSAmp1 = 0;
    int modSAmp2 = 0;

    int modSLfo1Rate = 0;
    int modSLfo1Depth = 0;
    int modSLfo1Delay = 0;

    int modSFilter1 = 0;
    int modSFilter2 = 0;
    int modSFilter3 = 0;

    int modSPitch = 0;
    int modSAmp3 = 0;

    int modVPan1 = 0;
    int modVPan2 = 0;
    int modVPan3 = 0;

    int modVAmp1 = 0;
    int modVAmp2 = 0;

    int modVLfo1Rate = 0;
    int modVLfo1Depth = 0;
    int modVLfo1Delay = 0;

    int lfo1Wave = 0;
    int lfo2Wave = 0;

    // Voice
    //bool lfo1Desync = false;
    //int voiceAssign = 0;

    // Soft Pedal
    //int softLoudness = 0;
    //int softAttack = 0;
    //int softFilter = 0;

    //double tune = 0.0;

    std::array<int8_t, 12> temperament{};

    std::vector<Keygroup> keygroups;

    std::vector<uint8_t> rawData;
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

    double tune = 0.0;

    uint32_t location = 0;
    uint64_t length = 0;
    uint32_t start = 0;
    uint32_t end = 0;
    uint8_t spare = 0;
    uint8_t waveComment = 0;
    uint16_t stereoPartner = 0;
    uint16_t sampleRate = 0;
    int8_t holdLoopTune = 0;

    struct Loop
    {
        uint32_t position = 0;
        double  length = 0.0;
        uint16_t dwell = 0;
        uint32_t relativeFactors = 0;
    };

    std::array<Loop, 4> loops;
    std::vector<uint8_t> rawData;
};


struct KeyZone
{
    uint16_t sampleId;
};