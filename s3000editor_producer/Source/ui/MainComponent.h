#pragma once

#include <JuceHeader.h>
#include "../s3000/S3000Types.h"
#include <map>
#include "ProgramTree.h"
#include "SampleHeaderEditor.h"
#include "VelocityZoneEditor.h"
#include "KeyGroupEditor.h"
#include "ProgramEditor.h"
#include "../midi/SysExSender.h"
#include "../midi/MidiManager.h"






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

    KeyGroupEditor keyGroupEditor;

    juce::TextButton requestButton{ "Request PLIST" };
//    void sendRPLIST();
    /*void sendRPDATA(int programIndex);*/
//    void sendProgramHeader(int programIndex);
//    void sendKGHeader(int programIndex, int keygroup);
    //void sendSampleHeader(int program, int keygroup);
    void sendSampleHeader(
        //int program,
        int sampleId);
//    void sendKData(
//        int programIndex,
//        int keygroup);

//    void sendKeygroupData(
//        int programIndex,
//        int keygroupIndex,
//        const std::vector<uint8_t>& data);
    
    SysExSender sysExSender;

    /*void parseRPDATA(const juce::MemoryBlock& data);*/
    void parseRPDATA(const std::vector<uint8_t>& decoded);
    void parseProgram(const std::vector<uint8_t>& decoded);
    void parseKeygroup(const std::vector<uint8_t>& decoded);
    void parsePLIST(const std::vector<uint8_t>& d);
    void sendRSLIST();
    std::queue<int> pendingSampleRequests;

    void dumpKeygroup(const std::vector<uint8_t>& decoded);
    Keygroup parseKeygroupStruct(const std::vector<uint8_t>& d);

    uint8_t unpack7bit(const std::vector<uint8_t>& d, int& bitPos);
    std::vector<uint8_t> decodeProgramHeader(
        const juce::MemoryBlock& data);

    SampleHeaderEditor sampleHeaderEditor;
    VelocityZoneEditor velocityZoneEditor;

    //enum class RequestType
    //{
    //    None,
    //    ProgramHeader,
    //    KeygroupHeader,
    //    KeygroupData,
    //    SampleHeader
    //};


    //RequestType currentRequest =
    //    RequestType::None;

    std::vector<ProgramEntry> programList;
    //std::map<uint16_t, SampleHeader> sampleHeaders;




    juce::TextButton requestRPDATAButton{ "get program" };

    juce::Viewport sampleHeaderViewport;
    juce::Viewport keyGroupViewport;


    juce::ListBox listBox;

    void resolveAllSampleIds();

    juce::MemoryBlock lastSysEx;
    juce::MemoryBlock sysExBuffer;
    juce::MemoryBlock programBuffer;

    std::vector<uint8_t> decodeNibbleData(
        const juce::MemoryBlock& data);

    std::vector<uint8_t> decodeKeygroupHeader(const juce::MemoryBlock& data);
    std::vector<uint8_t> decodeKeygroupFull(
        const juce::MemoryBlock& data);
    std::vector<uint8_t> decodeSampleHeader(
        const juce::MemoryBlock& data);


    std::vector<uint8_t> decodeKeygroupNibbleData(
        const juce::MemoryBlock& data);

    std::vector<uint8_t> decodeKData(
        const uint8_t* data,
        size_t size);

    /*std::vector<uint8_t>
        MainComponent::decodeProgramHeader(
            const juce::MemoryBlock& data);*/

    
    //std::vector<SampleHeader> samples;
    std::map<int, SampleHeader> sampleHeaders;
    std::map<int, juce::String> sampleList;
    std::map<int, juce::String> residentSamples;
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
    char decodePlistChar(uint8_t v);

    void parseSLIST(
        const juce::MemoryBlock& data);

    int currentRequestedSampleIndex = -1;

    //int currentKeygroup = -1;
    int currentZone = -1;



//    std::unique_ptr<juce::MidiInput> midiInput;
//    std::unique_ptr<juce::MidiOutput> midiOutput;
    std::string decodeName(const uint8_t* p);
    std::string trimRightSpaces(std::string s);

    bool inSysEx = false;
    bool inRPDATA = false;
    /*ProgramParams parseParams(const std::vector<uint8_t>& d, int panOffset,
        int freqOffset,
        int filterOffset);*/

    Program parseParams(const std::vector<uint8_t>& d);

    int findSampleId(
        const juce::String& name);

    int getNumRows() override;
    void paintListBoxItem(int rowNumber,
        juce::Graphics& g,
        int width, int height,
        bool rowIsSelected) override;

    void listBoxItemClicked(int row, const juce::MouseEvent&);

    Program loadedProgram;     // 受信したProgramデータ

    //int currentProgram = 0;    // 選択中のProgram番号

    int currentKeygroup = 0;
    int totalKeygroups = 0;

    juce::TreeView treeView;
    ProgramTree programTree;
    Program currentProgramData;
    int currentProgramIndex = 0;

    juce::TabbedComponent editorTabs{
    juce::TabbedButtonBar::TabsAtTop
    };

    ProgramEditor programEditor;
    juce::Viewport programEditorViewport;
    
    MidiManager midiManager;
    void processIncomingSysEx(
                               const juce::MidiMessage& message);
    void handleCommandReply(
        const uint8_t* data,
        int size);
    
    void handleProgramHeaderResponse();
    void handleKeygroupHeaderResponse();
    void handleKeygroupDataResponse(
        const juce::MidiMessage& message);
    void handleKeygroupFullResponse();
    void handleRPDataResponse();
    void handleSampleHeaderResponse();
    void handleSampleListResponse();
    void handleProgramListResponse(
        const uint8_t* data,
        int size);
//    void handleSampleHeaderResponse();
    
//    SysExSender sysExSender;
    
//    SysExSender sysExSender;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
// write-test
