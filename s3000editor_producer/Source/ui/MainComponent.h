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
#include "KeygroupMap.h"
#include <deque>
#include <set>






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


class S3000LookAndFeel : public juce::LookAndFeel_V4
{
public:
    S3000LookAndFeel()
    {
        const auto panel =
            juce::Colour::fromRGB(198, 197, 191);

        const auto panelLight =
            juce::Colour::fromRGB(218, 216, 208);

        const auto text =
            juce::Colour::fromRGB(35, 35, 35);

        const auto akaiRed =
            juce::Colour::fromRGB(190, 45, 38);

        setColour(
            juce::ResizableWindow::backgroundColourId,
            panel
        );

        setColour(
            juce::Label::textColourId,
            text
        );

        setColour(
            juce::TextEditor::backgroundColourId,
            panelLight
        );

        setColour(
            juce::TextEditor::textColourId,
            text
        );

        setColour(
            juce::TextEditor::outlineColourId,
            juce::Colour::fromRGB(130, 130, 125)
        );

        setColour(
            juce::TextEditor::focusedOutlineColourId,
            akaiRed
        );

        setColour(
            juce::ComboBox::backgroundColourId,
            panelLight
        );

        setColour(
            juce::ComboBox::textColourId,
            text
        );

        setColour(
            juce::ComboBox::outlineColourId,
            juce::Colour::fromRGB(130, 130, 125)
        );

        setColour(
            juce::ComboBox::arrowColourId,
            text
        );

        setColour(
            juce::TextButton::buttonColourId,
            juce::Colour::fromRGB(205, 203, 196)
        );

        setColour(
            juce::TextButton::buttonOnColourId,
            akaiRed
        );

        setColour(
            juce::TextButton::textColourOffId,
            text
        );

        setColour(
            juce::TextButton::textColourOnId,
            juce::Colours::white
        );

        setColour(
            juce::Slider::rotarySliderFillColourId,
            akaiRed
        );

        setColour(
            juce::Slider::rotarySliderOutlineColourId,
            juce::Colour::fromRGB(80, 80, 78)
        );

        setColour(
            juce::Slider::thumbColourId,
            juce::Colour::fromRGB(245, 244, 238)
        );

        setColour(
            juce::ToggleButton::textColourId,
            text
        );

        setColour(
            juce::ToggleButton::tickColourId,
            akaiRed
        );

        setColour(
            juce::TreeView::backgroundColourId,
            panelLight
        );

        setColour(
            juce::TabbedButtonBar::tabTextColourId,
            juce::Colour::fromRGB(55, 55, 52)
        );

        setColour(
            juce::TabbedButtonBar::frontTextColourId,
            juce::Colours::white
        );

        setColour(
            juce::TabbedComponent::backgroundColourId,
            juce::Colour::fromRGB(198, 197, 191)
        );

        setColour(
            juce::TabbedComponent::outlineColourId,
            juce::Colour::fromRGB(150, 149, 144)
        );

        setColour(
            juce::TabbedButtonBar::frontOutlineColourId,
            juce::Colour::fromRGB(190, 45, 38)
        );

        setColour(
            juce::ScrollBar::thumbColourId,
            juce::Colour::fromRGB(130, 129, 124)
        );

        setColour(
            juce::ScrollBar::backgroundColourId,
            juce::Colour::fromRGB(218, 216, 208)
        );

    }

    void drawTabButton(
        juce::TabBarButton& button,
        juce::Graphics& g,
        bool isMouseOver,
        bool isMouseDown) override
    {
        auto bounds =
            button.getActiveArea().toFloat();

        const bool isSelected =
            button.getToggleState();

        if (isSelected)
        {
            g.setColour(
                juce::Colour::fromRGB(190, 45, 38)
            );
        }
        else
        {
            g.setColour(
                juce::Colour::fromRGB(190, 189, 183)
            );
        }

        g.fillRect(bounds);

        g.setColour(
            isSelected
            ? juce::Colours::white
            : juce::Colour::fromRGB(55, 55, 52)
        );

        g.setFont(13.0f);

        g.drawText(
            button.getButtonText(),
            bounds,
            juce::Justification::centred
        );
    }

};



//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::Component,
    public juce::MidiInputCallback,
    public juce::ListBoxModel,
    public juce::MidiKeyboardStateListener,
    private juce::Timer,
    public juce::KeyListener,
    public juce::MenuBarModel
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    

    void handleIncomingMidiMessage(
        juce::MidiInput* source,
        const juce::MidiMessage& message);

    bool keyPressed(
        const juce::KeyPress& key,
        juce::Component* originatingComponent
    ) override;

    bool keyStateChanged(
        bool isKeyDown,
        juce::Component* originatingComponent
    ) override;


private:
    //==============================================================================
    // Your private member variables go here...
    S3000LookAndFeel s3000LookAndFeel;



    juce::MenuBarComponent menuBar{ this };

    juce::StringArray getMenuBarNames() override;

    juce::PopupMenu getMenuForIndex(
        int topLevelMenuIndex,
        const juce::String& menuName
    ) override;

    void menuItemSelected(
        int menuItemID,
        int topLevelMenuIndex
    ) override;

    void addProgram();
    void deleteProgram();

    void openKeyboardMidiInput(int index);


    juce::TextButton captureAButton{ "capture A" };
    juce::TextButton captureBButton{ "capture B" };
    juce::TextButton compareButton{ "compare" };
    
    juce::TextButton mockEnv2R1Button { "Mock ENV2 R1" };

    void timerCallback() override;

    double lastDeviceResponseTime = 0.0;
    bool deviceConnected = false;
    int heartbeatCounter = 0;

    //bool waitingForDeleteReply = false;

    //KeygroupMap keygroupMap;
    int pendingDeleteKeygroup = -1;
    bool waitingForDeleteReply = false;
    int pendingSampleRenameReadbackId = -1;

    enum class PendingAddStage
    {
        none,
        waitingForKeygroupReply,
        waitingForProgramReply
    };

    PendingAddStage pendingAddStage =
        PendingAddStage::none;

    Keygroup pendingAddedKeygroup;
    int pendingAddKeygroupIndex = -1;

    bool waitingForSampleRenameReply = false;
    int pendingRenamedSampleId = -1;


    KeyGroupEditor keyGroupEditor;

    void addKeygroup();

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
    //std::queue<int> pendingSampleRequests;


    std::set<int> pendingSampleRequests;
    std::deque<int> pendingSampleRequestOrder;

    int activeSampleHeaderRequestId = -1;

    void trySendNextSampleHeader();

    juce::ComboBox keyboardMidiInputCombo;
    juce::Label keyboardMidiInputLabel;

    std::unique_ptr<juce::MidiInput>
        keyboardMidiInput;

    juce::Array<juce::MidiDeviceInfo>
        keyboardMidiInputDevices;


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
    
    void injectTestKeygroupHeaderByte(
        int programNumber,
        int keygroupIndex,
        int offset,
        uint8_t value
    );
    




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
    //juce::Label programLabel;

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

    void handleNoteOn(
        juce::MidiKeyboardState* source,
        int midiChannel,
        int midiNoteNumber,
        float velocity) override;

    void handleNoteOff(
        juce::MidiKeyboardState* source,
        int midiChannel,
        int midiNoteNumber,
        float velocity) override;

    int currentResidentProgramIndex = -1;




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

    // UIで現在選択しているKeygroup
    int currentKeygroup = 0;

    // SysExで現在ロード中のKeygroup
    int loadingKeygroup = 0;

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
    juce::Viewport programViewport;
    
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

    juce::ComboBox programCombo;

    std::vector<juce::String> programNames;
    int currentProgram = -1;

    void loadProgram(int programIndex);

    juce::Viewport keygroupMapViewport;
    KeygroupMap keygroupMap;

    juce::Label keygroupTitleLabel;

    juce::TextButton addKeygroupButton{
        "+ Add KG"
    };

    juce::TextButton deleteKeygroupButton{
        "Delete KG"
    };
    juce::Label deviceStatusLabel;

    //bool deviceConnected = false;
    bool hasConnectedOnce = false;
    bool programRefreshOnly = false;
    bool initialProgramLoad = true;



    void setDeviceConnected(bool connected);
    double lastShortProgramChangeTime = 0.0;
    bool programRefreshPending = false;

    juce::TextButton auditionButton{ "AUDITION" };

    juce::MidiKeyboardState keyboardState;

    juce::MidiKeyboardComponent keyboardComponent
    {
        keyboardState,
        juce::MidiKeyboardComponent::horizontalKeyboard
    };

    juce::ToggleButton keyboardToggle{ "Keyboard" };

    juce::Slider keyboardVelocitySlider;

    juce::ComboBox keyboardOctaveCombo;
    juce::Label keyboardVelocityLabel;
    juce::Label keyboardOctaveLabel;

    int basicMidiChannel = 15;  // Akai raw: 0-15

    juce::TextButton deleteProgramButton{ "-" };

    juce::TextButton addProgramButton{ "+" };

    enum class PendingAddProgramStage
    {
        none,
        waitingForProgramReply,
        waitingForKeygroupReply
    };

    PendingAddProgramStage pendingAddProgramStage =
        PendingAddProgramStage::none;

    int pendingNewProgramNumber = -1;

    Program pendingNewProgram;
    Keygroup pendingNewProgramKeygroup;

    int pendingProgramSelection = -1;

    bool restoringKeygroupDisplay = false;

    juce::TextButton saveProjectButton{ "Save Project" };
    void saveProject();

    juce::TextButton loadProjectButton{ "Load Project" };
    void loadProject();

    bool waitingForFilterReply = false;
    int pendingFilterFreqValue = -1;
    int pendingFilterFreqKeygroup = -1;

    int pendingResonanceValue = -1;
    int pendingResonanceKeygroup = -1;

    int pendingEnv2R1Value = -1;
    int pendingEnv2R1Keygroup = -1;

    int pendingEnv2L1Value = -1;
    int pendingEnv2L1Keygroup = -1;

    int pendingModFilter1Value = -1000;
    int pendingModFilter1Keygroup = -1;

    int pendingEnv2R2Value = -1;
    int pendingEnv2R2Keygroup = -1;

    int pendingEnv2L2Value = -1;
    int pendingEnv2L2Keygroup = -1;

    int pendingEnv2R3Value = -1;
    int pendingEnv2R3Keygroup = -1;

    int pendingEnv2L3Value = -1;
    int pendingEnv2L3Keygroup = -1;

    int pendingEnv2R4Value = -1;
    int pendingEnv2R4Keygroup = -1;

    int pendingEnv2L4Value = -1;
    int pendingEnv2L4Keygroup = -1;

    int pendingEnv1AttackValue = -1;
    int pendingEnv1AttackKeygroup = -1;

    int pendingEnv1DecayValue = -1;
    int pendingEnv1DecayKeygroup = -1;

    int pendingEnv1SustainValue = -1;
    int pendingEnv1SustainKeygroup = -1;

    int pendingEnv1ReleaseValue = -1;
    int pendingEnv1ReleaseKeygroup = -1;






    int programToRestoreAfterReconnect = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
// write-test
