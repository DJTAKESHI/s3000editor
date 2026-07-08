#pragma once

#include <JuceHeader.h>



struct Keygroup;

struct Program
{
    int programNumber;
    std::string name;
    int midiChannel;
    int polyphony;

    std::vector<Keygroup> keygroups;
};


struct PlaybackControl
{
    int kbeat;
    bool ahold;

    bool constantPitch;

    int keygroupXfade;

};

struct Filter1
{
    int freq;
    int keyFollow;
    int resonannce;
};

struct Envelope
{
    int attack;
    int decay;
    int sustain;
    int release;

    int velAttack;
    int velRelease;
    int noteOffRelease;
    int keyTracking;
};


Keygroup parseKeygroup(const std::vector<uint8_t>& d);

struct VelocityZone
{
    std::string sampleName;

    int lowVel;
    int highVel;

    int tune;
    int loudness;
    int pan;

    int playMode;

};

struct PLISTEntry
{
    int index = -1;
    juce::String name;
};

struct ProgramEntry
{
    int index;
    std::string name;
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

struct ModSlot
{
    ModSource source = ModSource::Off;
    ModDestination dest = ModDestination::None;

    int amount = 0;

};

struct ModContext
{
    float lfo1;
    float lfo2;
    float env1;
    float env2;
    float velocity;
    float aftertouch;
    float modWheel;
    float keytrack;
    float pitchBend;

    float get(ModSource s) const;
};

struct ModMatrix
{
    std::array<ModSlot, 6> slots;
};

float evaluateModMatrix(const ModMatrix& mod, const ModContext& ctx);


struct Keygroup
{
    // === block ===
    //int id;
    //int nextBlock;

    // === key range ===
    int lowNote;
    int highNote;

    // === tuning ===
    int tune;

    // === filter ===
    Filter1 filter1;


    Envelope env1;
    Envelope env2;



    std::array<VelocityZone, 4> zones;

    PlaybackControl playback;

    ModMatrix mod;



};


std::optional<int> findPanOffset(const std::vector<uint8_t>& a,
    const std::vector<uint8_t>& b,
    const std::vector<uint8_t>& c);

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component,
    public juce::MidiInputCallback, public juce::ListBoxModel
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    

    void handleIncomingMidiMessage(juce::MidiInput* source,
        const juce::MidiMessage& message) override;

private:
    //==============================================================================
    // Your private member variables go here...
    juce::TextButton captureAButton{ "capture A" };
    juce::TextButton captureBButton{ "capture B" };
    juce::TextButton compareButton{ "compare" };

    juce::TextButton requestButton{ "Request PLIST" };
    void sendRPLIST();
    void sendRPDATA(int programIndex);
    /*void parseRPDATA(const juce::MemoryBlock& data);*/
    void parseRPDATA(const std::vector<uint8_t>& decoded);
    void parseProgram(const std::vector<uint8_t>& decoded);
    void parseKeygroup(const std::vector<uint8_t>& decoded);
    void parsePLIST(const std::vector<uint8_t>& d);

    void dumpKeygroup(const std::vector<uint8_t>& decoded);
    Keygroup parseKeygroupStruct(const std::vector<uint8_t>& d);

    uint8_t unpack7bit(const std::vector<uint8_t>& d, int& bitPos);

    std::vector<ProgramEntry> programList;




    juce::TextButton requestRPDATAButton{ "get program" };

    juce::ListBox listBox;

    juce::MemoryBlock lastSysEx;
    juce::MemoryBlock sysExBuffer;
    juce::MemoryBlock programBuffer;

    std::vector<uint8_t> decodeNibbleData(
        const juce::MemoryBlock& data);

    juce::Label programLabel;

    juce::File dumpA;
    juce::File dumpB;
    juce::File rawFileA, rawFileB;
    juce::File decodedFileA, decodedFileB;

    bool isCaptureA = true;

    void saveDump(const juce::String& name);
    void saveRawDump(const juce::String& name);
    void saveDecodedDump(const juce::String& name, 
        const std::vector<uint8_t>& decoded);
    void saveRawRPDATA(const juce::String& name, const juce::MemoryBlock& data);

    void compareLatest();

    int dumpState = 0;
    void compareDumps(const juce::File& fileA, const juce::File& fileB);

    void saveRawSysEx(const uint8_t* data, size_t size);
    char MainComponent::decodePlistChar(uint8_t v);



    std::unique_ptr<juce::MidiInput> midiInput;
    std::unique_ptr<juce::MidiOutput> midiOutput;
    std::string decodeName(const uint8_t* p);
    std::string trimRightSpaces(std::string s);

    bool inSysEx = false;
    bool inRPDATA = false;
    /*ProgramParams parseParams(const std::vector<uint8_t>& d, int panOffset,
        int freqOffset,
        int filterOffset);*/

    Program parseParams(const std::vector<uint8_t>& d);

    int getNumRows() override;
    void paintListBoxItem(int rowNumber,
        juce::Graphics& g,
        int width, int height,
        bool rowIsSelected) override;

    void listBoxItemClicked(int row, const juce::MouseEvent&);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
