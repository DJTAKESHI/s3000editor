#pragma once

#include <JuceHeader.h>
#include "../s3000/S3000Types.h"



struct Keygroup;



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
    void sendKGHeader(int programIndex, int keygroup);
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

    std::vector<uint8_t> MainComponent::decodeKeygroupHeader(const juce::MemoryBlock& data);


    std::vector<uint8_t> decodeKeygroupNibbleData(
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
