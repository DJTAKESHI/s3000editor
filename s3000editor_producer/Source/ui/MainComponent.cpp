#include "MainComponent.h"
#include <fstream>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../s3000/Offsets.h"
#include "../s3000/ProgramParser.h"
#include "../s3000/KeygroupParser.h"
#include "../s3000/SampleHeaderParser.h"
//#include "../s3000/KeygroupHeaderParser.h"
#include "../s3000/S3000Types.h"
#include "../s3000/SampleHeaderEncoder.h"
#include "../s3000/KeygroupEncoder.h"
#include "../s3000/ProgramEncoder.h"

char decodeAkaiChar(uint8_t v)
{
    // space / padding
    if (v == 0x0A || v == 0x00)
        return ' ';

    // printable ASCII fallback�i���d�v�E
    if (v >= 0x20 && v <= 0x7E)
        return (char)v;

    // unknown control
    return '?';
}

//

//==============================================================================
MainComponent::MainComponent()
{
    setLookAndFeel(&s3000LookAndFeel);

    setSize(900, 800);

    addAndMakeVisible(listBox);
    listBox.setModel(this);

    addAndMakeVisible(programTree);

    addAndMakeVisible(editorTabs);
    addAndMakeVisible(menuBar);

    addAndMakeVisible(auditionButton);
    auditionButton.setAlwaysOnTop(true);
    //addAndMakeVisible(mockEnv2R1Button);
    //
    //mockEnv2R1Button.onClick =
    //    [this]()
    //    {
    //        if (loadedProgram.keygroups.empty())
    //        {
    //            loadedProgram.keygroups.resize(1);

    //            keyGroupEditor.setKeygroup(
    //                loadedProgram.keygroups[0],
    //                0
    //            );

    //            DBG("CREATED MOCK KEYGROUP 0");
    //        }

    //        injectTestKeygroupHeaderByte(
    //            0,
    //            0,
    //            157,
    //            60
    //        );
    //    };

    auditionButton.onClick = [this]()
        {
            DBG("AUDITION CLICKED");

            sysExSender.sendNoteOn(
                60,
                100,
                16
            );

            juce::Timer::callAfterDelay(
                500,
                [this]()
                {
                    sysExSender.sendNoteOff(
                        60,
                        16
                    );
                }
            );
        };


    juce::PropertiesFile::Options options;

    options.applicationName = "S3000XLEditor";
    options.filenameSuffix = "settings";
    options.osxLibrarySubFolder =
        "Application Support";

    midiProperties =
        std::make_unique<juce::PropertiesFile>(
            options
        );
   

    keygroupMap.onKeygroupSelected =
        [this](int index)
        {
            if (index < 0 ||
                index >=
                static_cast<int>(
                    loadedProgram.keygroups.size()
                    ))
            {
                return;
            }

            currentKeygroup = index;
            currentZone = 0;

            const int modVPitch =
                loadedProgram
                .keygroups[currentKeygroup]
                .modVPitch;


            const int modFilter1 =
                loadedProgram
                .keygroups[currentKeygroup]
                .modFilter1;

            const int modFilter2 =
                loadedProgram
                .keygroups[currentKeygroup]
                .modFilter2;

            const int modFilter3 =
                loadedProgram
                .keygroups[currentKeygroup]
                .modFilter3;


            programEditor.setModPitchAmount(
                modVPitch
            );

            programEditor.setModFilter1Amount(
                modFilter1
            );

            programEditor.setModFilter2Amount(
                modFilter2
            );

            programEditor.setModFilter3Amount(
                modFilter3
            );


            const int lfo1Pitch =
                loadedProgram
                .keygroups[currentKeygroup]
                .lfo1Pitch;



            programEditor.setLfo1PitchAmount(
                lfo1Pitch
            );


            const auto& kg =
                loadedProgram.keygroups[index];


            keyGroupEditor.setKeygroup(
                kg,
                index
            );

 

            velocityZoneEditor.setZones(
                kg.zones
            );



            if (!kg.zones.empty())
            {
                velocityZoneEditor.setZone(
                    kg.zones[0]
                );

                const int sampleId =
                    kg.zones[0].sampleId;

                auto it =
                    sampleHeaders.find(
                        sampleId
                    );

                if (it != sampleHeaders.end())
                {
                    sampleHeaderEditor
                        .setSampleHeader(
                            it->second
                        );
                }
            }


        };

    addAndMakeVisible(keygroupMapViewport);

    keygroupMapViewport.setViewedComponent(
        &keygroupMap,
        false
    );

    keygroupMapViewport.setScrollBarsShown(
        true,    // vertical
        false    // horizontal
    );



    sampleHeaderViewport.setViewedComponent(
        &sampleHeaderEditor,
        false
    );

    keyGroupViewport.setViewedComponent(
        &keyGroupEditor,
        false
    );

    keyGroupViewport.setScrollBarsShown(
        true,   // vertical
        false    // horizontal
    );

    addAndMakeVisible(programViewport);

    programViewport.setViewedComponent(
        &programEditor,
        false
    );

    programViewport.setScrollBarsShown(
        true,
        false
    );

    programViewport.setScrollBarsShown(
        true,
        false
    );

    editorTabs.addTab(
        "Program",
        juce::Colours::darkgrey,
        &programViewport,
        false
    );

    editorTabs.addTab(
        "Keygroup",
        juce::Colours::darkgrey,
        &keyGroupViewport,
        false
    );

    editorTabs.addTab(
        "Velocity Zone",
        juce::Colours::darkgrey,
        &velocityZoneEditor,
        false
    );

    editorTabs.addTab(
        "Sample",
        juce::Colours::darkgrey,
        &sampleHeaderViewport,
        false
    );

    editorTabs.setCurrentTabIndex(0);

    programEditor.onModFilter1SourceChanged =
        [this](int value)
        {

            sysExSender.sendProgramHeaderByte(
                loadedProgram.programNumber,
                ProgramOffset::Mod::ModSFilter1,
                value
            );

            loadedProgram.modSFilter1 = value;
        };

    programEditor.onModFilter2SourceChanged =
        [this](int value)
        {

            sysExSender.sendProgramHeaderByte(
                loadedProgram.programNumber,
                ProgramOffset::Mod::ModSFilter2,
                value
            );

            loadedProgram.modSFilter2 = value;
        };

    programEditor.onModFilter3SourceChanged =
        [this](int value)
        {

            sysExSender.sendProgramHeaderByte(
                loadedProgram.programNumber,
                ProgramOffset::Mod::ModSFilter3,
                value
            );

            loadedProgram.modSFilter3 = value;
        };

    programEditor.onPortamentoEnableChanged =
        [this](bool enabled)
        {
            const int value =
                enabled ? 1 : 0;


            sysExSender.sendProgramHeaderByte(
                loadedProgram.programNumber,
                ProgramOffset::Portamento::Enable,
                value
            );

            loadedProgram.portamentoEnabled =
                enabled;
        };

    programEditor.onPortamentoTypeChanged =
        [this](int value)
        {

            sysExSender.sendProgramHeaderByte(
                loadedProgram.programNumber,
                ProgramOffset::Portamento::Type,
                value
            );

            loadedProgram.portamentoType =
                value;
        };

    programEditor.onPortamentoValueChanged =
        [this](int value)
        {

            sysExSender.sendProgramHeaderByte(
                loadedProgram.programNumber,
                ProgramOffset::Portamento::Time,
                value
            );

            loadedProgram.portamentoTime =
                value;
        };

    programEditor.onProgramChanged =
        [this](
            const Program& program)
        {

            const bool nameChanged =
                program.name != loadedProgram.name;

            if (nameChanged)
            {
                const auto newName =
                    juce::String(program.name)
                    .trim()
                    .toUpperCase();

                for (const auto& entry : programList)
                {
                    if (entry.index == currentProgram)
                        continue;

                    if (juce::String(entry.name)
                        .trim()
                        .equalsIgnoreCase(newName))
                    {

                        programEditor.setProgram(
                            loadedProgram
                        );

                        return;
                    }
                }
            }
            
            Program programToSend =
                program;

            programToSend.portamentoTime =
                loadedProgram.portamentoTime;

            programToSend.portamentoType =
                loadedProgram.portamentoType;

            programToSend.portamentoEnabled =
                loadedProgram.portamentoEnabled;

            auto encodedProgram =
                ProgramEncoder::encode(
                    programToSend
                );



            if (encodedProgram.empty())
            {
                DBG(
                    "PROGRAM EDITOR ENCODE FAILED"
                );

                return;
            }

            programRefreshOnly = true;


            DBG(
                "PDATA CALLER 441"
                " PAN="
                + juce::String(program.pan)
            );


            sysExSender.sendProgramData(
                program.programNumber,
                encodedProgram
            );

            // 診断用
            //sysExSender.sendProgramHeader(
            //    program.programNumber
            //);

            if (nameChanged)
            {
                loadedProgram.name =
                    program.name;

                DBG(
                    "PROGRAM RENAMED TO ["
                    + juce::String(loadedProgram.name)
                    + "]"
                );

                sysExSender.sendRPLIST();
            }

            loadedProgram.playLow =
                program.playLow;

            loadedProgram.playHigh =
                program.playHigh;

            // Modulation
            loadedProgram.modSPan1 = program.modSPan1;
            loadedProgram.modSPan2 = program.modSPan2;
            loadedProgram.modSPan3 = program.modSPan3;

            loadedProgram.modSAmp1 = program.modSAmp1;
            loadedProgram.modSAmp2 = program.modSAmp2;
            loadedProgram.modSAmp3 = program.modSAmp3;

            loadedProgram.modSLfo1Rate = program.modSLfo1Rate;
            loadedProgram.modSLfo1Depth = program.modSLfo1Depth;
            loadedProgram.modSLfo1Delay = program.modSLfo1Delay;

            loadedProgram.modSFilter1 = program.modSFilter1;
            loadedProgram.modSFilter2 = program.modSFilter2;
            loadedProgram.modSFilter3 = program.modSFilter3;

            loadedProgram.modSPitch = program.modSPitch;

            loadedProgram.modVPan1 = program.modVPan1;
            loadedProgram.modVPan2 = program.modVPan2;
            loadedProgram.modVPan3 = program.modVPan3;

            loadedProgram.modVAmp1 = program.modVAmp1;
            loadedProgram.modVAmp2 = program.modVAmp2;

            loadedProgram.modVLfo1Rate = program.modVLfo1Rate;
            loadedProgram.modVLfo1Depth = program.modVLfo1Depth;
            loadedProgram.modVLfo1Delay = program.modVLfo1Delay;
            loadedProgram.modVPitch = program.modVPitch;
        };

        programEditor.onModPitchSourceChanged =
            [this](int value)
            {

                sysExSender.sendProgramHeaderByte(
                    loadedProgram.programNumber,
                    ProgramOffset::Mod::ModSPitch,
                    value
                );

                loadedProgram.modSPitch = value;


            };

        programEditor.onModFilter1Changed =
            [this](int value)
            {
                if (currentKeygroup < 0 ||
                    currentKeygroup >=
                    static_cast<int>(
                        loadedProgram.keygroups.size()))
                {
                    return;
                }

                // -------------------------
                // Local model は即座に更新
                // -------------------------

                if (programEditor.isModulationEditAll())
                {
                    for (auto& keygroup :
                        loadedProgram.keygroups)
                    {
                        keygroup.modFilter1 = value;
                    }
                }
                else
                {
                    loadedProgram
                        .keygroups[currentKeygroup]
                        .modFilter1 = value;
                }

                const int targetKeygroup =
                    programEditor.isModulationEditAll()
                    ? 0x7f
                    : currentKeygroup;

                // -------------------------
                // ACK待ち中なら最新値だけ保持
                // -------------------------

                if (waitingForFilterReply)
                {
                    pendingModFilter1Value = value;
                    pendingModFilter1Keygroup =
                        targetKeygroup;

                    DBG(
                        "MOD FILTER1 PENDING"
                        " KG="
                        + juce::String(targetKeygroup)
                        + " VALUE="
                        + juce::String(value)
                    );

                    return;
                }

                // -------------------------
                // 今は空いているので送信
                // -------------------------

                waitingForFilterReply = true;

                DBG(
                    "MOD FILTER1 SEND"
                    " KG="
                    + juce::String(targetKeygroup)
                    + " VALUE="
                    + juce::String(value)
                );

                sysExSender.sendKeygroupHeaderByte(
                    loadedProgram.programNumber,
                    targetKeygroup,
                    KeygroupHeaderOffset::Mod::Filter1,
                    value
                );
            };

        programEditor.onModFilter2Changed =
            [this](int value)
            {
                if (currentKeygroup < 0 ||
                    currentKeygroup >=
                    static_cast<int>(
                        loadedProgram.keygroups.size()))
                {
                    return;
                }

                if (programEditor.isModulationEditAll())
                {
                    for (auto& keygroup : loadedProgram.keygroups)
                        keygroup.modFilter2 = value;
                }
                else
                {
                    loadedProgram.keygroups[currentKeygroup]
                        .modFilter2 = value;
                }

                const int targetKeygroup =
                    programEditor.isModulationEditAll()
                    ? 0x7f
                    : currentKeygroup;

                if (waitingForFilterReply)
                {
                    pendingModFilter2Value = value;
                    pendingModFilter2Keygroup = targetKeygroup;

                    DBG(
                        "MOD FILTER2 PENDING KG="
                        + juce::String(targetKeygroup)
                        + " VALUE="
                        + juce::String(value)
                    );

                    return;
                }

                waitingForFilterReply = true;

                DBG(
                    "MOD FILTER2 SEND KG="
                    + juce::String(targetKeygroup)
                    + " VALUE="
                    + juce::String(value)
                );

                sysExSender.sendKeygroupHeaderByte(
                    loadedProgram.programNumber,
                    targetKeygroup,
                    KeygroupHeaderOffset::Mod::Filter2,
                    value
                );
            };

        programEditor.onModFilter3Changed =
            [this](int value)
            {
                if (currentKeygroup < 0 ||
                    currentKeygroup >=
                    static_cast<int>(
                        loadedProgram.keygroups.size()))
                {
                    return;
                }

                if (programEditor.isModulationEditAll())
                {
                    for (auto& keygroup : loadedProgram.keygroups)
                        keygroup.modFilter3 = value;
                }
                else
                {
                    loadedProgram.keygroups[currentKeygroup]
                        .modFilter3 = value;
                }

                const int targetKeygroup =
                    programEditor.isModulationEditAll()
                    ? 0x7f
                    : currentKeygroup;

                if (waitingForFilterReply)
                {
                    pendingModFilter3Value = value;
                    pendingModFilter3Keygroup = targetKeygroup;

                    DBG(
                        "MOD FILTER3 PENDING KG="
                        + juce::String(targetKeygroup)
                        + " VALUE="
                        + juce::String(value)
                    );

                    return;
                }

                waitingForFilterReply = true;

                DBG(
                    "MOD FILTER3 SEND KG="
                    + juce::String(targetKeygroup)
                    + " VALUE="
                    + juce::String(value)
                );

                sysExSender.sendKeygroupHeaderByte(
                    loadedProgram.programNumber,
                    targetKeygroup,
                    KeygroupHeaderOffset::Mod::Filter3,
                    value
                );
            };

        programEditor.onEnv2PitchChanged =
            [this](int value)
            {
                if (currentKeygroup < 0 ||
                    currentKeygroup >=
                    static_cast<int>(
                        loadedProgram.keygroups.size()))
                {
                    DBG("ENV2 PITCH: INVALID KEYGROUP");
                    return;
                }

                if (programEditor.isModulationEditAll())
                {
                    for (auto& keygroup :
                        loadedProgram.keygroups)
                    {
                        keygroup.modVPitch = value;
                    }


                    sysExSender.sendKeygroupHeaderByte(
                        loadedProgram.programNumber,
                        0x7f,
                        KeygroupHeaderOffset::Mod::ModVPitch,
                        value
                    );

                    return;
                }

                auto& keygroup =
                    loadedProgram.keygroups[currentKeygroup];

                keygroup.modVPitch = value;


                sysExSender.sendKeygroupHeaderByte(
                    loadedProgram.programNumber,
                    currentKeygroup,
                    KeygroupHeaderOffset::Mod::ModVPitch,
                    value
                );
            };



        programEditor.onLfo1PitchChanged =
            [this](int value)
            {
                if (currentKeygroup < 0 ||
                    currentKeygroup >=
                    static_cast<int>(
                        loadedProgram.keygroups.size()
                        ))
                {
                    DBG("LFO1 PITCH: INVALID KEYGROUP");
                    return;
                }

                auto& keygroup =
                    loadedProgram.keygroups[currentKeygroup];

                keygroup.lfo1Pitch = value;


                sysExSender.sendKeygroupHeaderByte(
                    loadedProgram.programNumber,
                    currentKeygroup,
                    KeygroupHeaderOffset::Mod::Lfo1Pitch,
                    keygroup.lfo1Pitch
                );
            };

    velocityZoneEditor.onZoneSelected =
        [this](int zoneIndex)
        {
            if (currentKeygroup < 0 ||
                currentKeygroup >=
                static_cast<int>(
                    loadedProgram.keygroups.size()
                    ))
            {
                return;
            }

            if (zoneIndex < 0 ||
                zoneIndex >= 4)
            {
                return;
            }

            currentZone =
                zoneIndex;

            auto& kg =
                loadedProgram.keygroups[
                    currentKeygroup
                ];

            // �I������Zone���ڍ�Editor��
            velocityZoneEditor.setZone(
                kg.zones[currentZone]
            );

            // Sample Header���؂�ւ���
            const int sampleId =
                kg.zones[currentZone].sampleId;

            auto it =
                sampleHeaders.find(
                    sampleId
                );

            if (it != sampleHeaders.end())
            {
                sampleHeaderEditor.setSampleHeader(
                    it->second
                );
            }


        };


    programTree.setRootItemVisible(true);
    programTree.setDefaultOpenness(true);

    programTree.onZoneSelected =
        [this](
            int keygroupIndex,
            int zoneIndex,
            const VelocityZone& zone,
            const SampleHeader& sampleHeader
            )
        {
            currentKeygroup = keygroupIndex;

            if (currentKeygroup >= 0
                && currentKeygroup < static_cast<int>(loadedProgram.keygroups.size()))
            {
                programEditor.setModPitchAmount(
                    loadedProgram.keygroups[currentKeygroup].modVPitch
                );
            }

            currentZone = zoneIndex;

            if (currentKeygroup >= 0 &&
                currentKeygroup <
                static_cast<int>(loadedProgram.keygroups.size()))
            {
                const int modVPitch =
                    loadedProgram
                    .keygroups[currentKeygroup]
                    .modVPitch;


                programEditor.setModPitchAmount(
                    modVPitch
                );
            }


            // ========================================
            // 4 Zone�S����Overview��
            // ========================================

            if (currentKeygroup >= 0 &&
                currentKeygroup <
                static_cast<int>(
                    loadedProgram.keygroups.size()
                    ))
            {


                velocityZoneEditor.setZones(
                    loadedProgram
                    .keygroups[currentKeygroup]
                    .zones
                );

            }

            // ========================================
            // �I��Zone���ڍ�Editor��
            // ========================================




            velocityZoneEditor.setZone(
                zone
            );

            sampleHeaderEditor.setSampleHeader(
                sampleHeader
            );

            editorTabs.setCurrentTabIndex(2);
        };

    addAndMakeVisible(programCombo);

    deviceStatusLabel.setColour(
        juce::Label::textColourId,
        juce::Colour::fromRGB(55, 55, 52)
    );

    deviceStatusLabel.setColour(
        juce::Label::textColourId,
        juce::Colour::fromRGB(55, 55, 52)
    );

    //addAndMakeVisible(loadProjectButton);

    loadProjectButton.onClick = [this]()
        {
            loadProject();
        };

    loadProjectButton.toFront(false);



    addAndMakeVisible(deviceStatusLabel);

    //addAndMakeVisible(saveProjectButton);

    saveProjectButton.onClick = [this]()
        {
            saveProject();
        };

    keyboardMidiInputLabel.setText(
        "Keyboard MIDI Input",
        juce::dontSendNotification
    );

    addAndMakeVisible(
        keyboardMidiInputLabel
    );

    addAndMakeVisible(
        keyboardMidiInputCombo
    );

    keyboardMidiInputDevices =
        juce::MidiInput::getAvailableDevices();

    for (int i = 0;
        i < keyboardMidiInputDevices.size();
        ++i)
    {
        keyboardMidiInputCombo.addItem(
            keyboardMidiInputDevices[i].name,
            i + 1
        );
    }

    keyboardMidiInputCombo.onChange =
        [this]()
        {
            const int index =
                keyboardMidiInputCombo
                .getSelectedId() - 1;

            openKeyboardMidiInput(index);
        };


    velocityZoneEditor.onZoneRangeChanged =
        [this](
            int zoneIndex,
            int low,
            int high
            )
        {
            if (currentKeygroup < 0 ||
                currentKeygroup >= static_cast<int>(
                    loadedProgram.keygroups.size()))
            {
                return;
            }

            if (zoneIndex < 0 ||
                zoneIndex >= 4)
            {
                return;
            }

            auto& kg =
                loadedProgram.keygroups[
                    currentKeygroup
                ];

            auto& zone =
                kg.zones[zoneIndex];

            const bool lowChanged =
                zone.lowVel != low;

            const bool highChanged =
                zone.highVel != high;

            zone.lowVel =
                static_cast<uint8_t>(low);

            zone.highVel =
                static_cast<uint8_t>(high);

            currentZone =
                zoneIndex;

            velocityZoneEditor.setZone(
                zone
            );

            // LOVEL1 = 46
            // HIVEL1 = 47
            // Zone stride = 24

            if (lowChanged)
            {
                const int offset =
                    46 + zoneIndex * 24;


                sysExSender.sendKeygroupByte(
                    currentProgramIndex,
                    currentKeygroup,
                    offset,
                    static_cast<uint8_t>(low)
                );
            }

            if (highChanged)
            {
                const int offset =
                    47 + zoneIndex * 24;


                sysExSender.sendKeygroupByte(
                    currentProgramIndex,
                    currentKeygroup,
                    offset,
                    static_cast<uint8_t>(high)
                );
            }
        };

    velocityZoneEditor.onVelocityRangeChanged =
        [this](int low, int high)
        {
            if (currentKeygroup < 0 ||
                currentKeygroup >= static_cast<int>(
                    loadedProgram.keygroups.size()))
            {
                return;
            }

            if (currentZone < 0 ||
                currentZone >= 4)
            {
                return;
            }

            auto& zone =
                loadedProgram
                .keygroups[currentKeygroup]
                .zones[currentZone];

            const bool lowChanged =
                zone.lowVel != low;

            const bool highChanged =
                zone.highVel != high;

            zone.lowVel = low;
            zone.highVel = high;

            velocityZoneEditor.setZones(
                loadedProgram
                .keygroups[currentKeygroup]
                .zones
            );

            // Zone1: LOVEL1=46, HIVEL1=47
            // Each following zone is +24 bytes.
            if (lowChanged)
            {
                const int offset =
                    46 + currentZone * 24;



                sysExSender.sendKeygroupByte(
                    currentProgramIndex,
                    currentKeygroup,
                    offset,
                    static_cast<uint8_t>(low)
                );
            }

            if (highChanged)
            {
                const int offset =
                    47 + currentZone * 24;



                sysExSender.sendKeygroupByte(
                    currentProgramIndex,
                    currentKeygroup,
                    offset,
                    static_cast<uint8_t>(high)
                );
            }
        };

    keygroupTitleLabel.setText(
        "KEYGROUPS",
        juce::dontSendNotification
    );

    keygroupTitleLabel.setColour(
        juce::Label::textColourId,
        juce::Colour::fromRGB(55, 55, 52)
    );

    keygroupTitleLabel.setFont(
        juce::Font(13.0f).boldened()
    );

    addAndMakeVisible(
        keygroupTitleLabel
    );



    addAndMakeVisible(
        addKeygroupButton
    );

    addAndMakeVisible(
        deleteKeygroupButton
    );

    addKeygroupButton.onClick =
        [this]()
        {
            addKeygroup();
        };


    deleteKeygroupButton.onClick =
        [this]()
        {
            if (loadedProgram.groups <= 1)
                return;

            if (currentKeygroup < 0 ||
                currentKeygroup >=
                static_cast<int>(
                    loadedProgram.keygroups.size()
                    ))
            {
                return;
            }

            pendingDeleteKeygroup =
                currentKeygroup;

            waitingForDeleteReply =
                true;

            sysExSender.sendDeleteKeygroup(
                loadedProgram.programNumber,
                currentKeygroup
            );
        };


    programTree.onBasicZoneSelected =
        [this](
            int keygroupIndex,
            int zoneIndex,
            const VelocityZone& zone
            )
        {

            currentKeygroup = keygroupIndex;
            currentZone = zoneIndex;

            // Velocity Zone�͕K���\��
            velocityZoneEditor.setZone(zone);

            // ==============================
            // Sample Header��ID����T��
            // ==============================

            if (zone.sampleId < 0)
            {
                DBG("EMPTY ZONE -> CLEAR SAMPLE HEADER EDITOR");

                sampleHeaderEditor.clear();
            }
            else
            {
                auto it =
                    sampleHeaders.find(
                        zone.sampleId
                    );

                if (it != sampleHeaders.end())
                {
  

                    sampleHeaderEditor.setSampleHeader(
                        it->second
                    );
                }
                else
                {

                    sendSampleHeader(
                        zone.sampleId
                    );
                }
            }

            editorTabs.setCurrentTabIndex(1);
        };


#if JUCE_DEBUG

    addAndMakeVisible(captureAButton);

    captureAButton.setButtonText(
        "TEST ADD KG"
    );

    captureAButton.onClick =
        [this]()
        {
            DBG("=== CAPTURE A CLICKED ===");
            auto testProgram =
                loadedProgram;

            testProgram.pan = -20;
            testProgram.loudness = 80;

            auto encodedProgram =
                ProgramEncoder::encode(
                    testProgram
                );

            if (encodedProgram.empty())
            {
                DBG("PROGRAM TEST ENCODE FAILED");
                return;
            }


            DBG("PDATA CALLER 1279"); 
            sysExSender.sendProgramData(
                loadedProgram.programNumber,
                encodedProgram
            );
        };


#endif







    DBG("========== BEFORE PROGRAM TREE ==========");

    for (int i = 0; i < currentProgramData.keygroups.size(); ++i)
    {
        auto& kg = currentProgramData.keygroups[i];

        DBG("KEYGROUP " + juce::String(i));

        for (int z = 0; z < kg.zones.size(); ++z)
        {
            auto& zone = kg.zones[z];


        }
    }

    programTree.onKeygroupSelected =
        [this](
            int keygroupIndex,
            const Keygroup& keygroup
            )
        {


            currentKeygroup = keygroupIndex;
            currentZone = 0;

            const int currentTab =
                editorTabs.getCurrentTabIndex();

            if (!initialProgramLoad)
            {
                if (currentTab == 0 ||
                    currentTab == 2)
                {
                    editorTabs.setCurrentTabIndex(1);
                }
            }

            keyGroupEditor.setKeygroup(
                loadedProgram.keygroups[keygroupIndex],
                keygroupIndex
            );


            velocityZoneEditor.setZones(
                loadedProgram.keygroups[keygroupIndex].zones
            );

            velocityZoneEditor.setSelectedZone(
                currentZone
            );

            velocityZoneEditor.setZone(
                loadedProgram.keygroups[keygroupIndex]
                .zones[currentZone]
            );
        };

   

#if JUCE_DEBUG

        // ========================================
        // TEST ADD KEYGROUP
        // ========================================

    addAndMakeVisible(captureAButton);

    captureAButton.setButtonText(
        "TEST ADD KG"
    );

    captureAButton.onClick =
        [this]()
        {
            auto testProgram =
                loadedProgram;

            testProgram.midiChannel = 5;

            auto encodedProgram =
                ProgramEncoder::encode(
                    testProgram
                );

            if (encodedProgram.empty())
            {
                DBG("PROGRAM TEST ENCODE FAILED");
                return;
            }


            DBG("PDATA CALLER 1388");
            sysExSender.sendProgramData(
                loadedProgram.programNumber,
                encodedProgram
            );
        };


    // ========================================
    // TEST DELETE KEYGROUP
    // ========================================

    addAndMakeVisible(captureBButton);

    captureBButton.setButtonText(
        "TEST DELETE KG"
    );

    captureBButton.onClick =
        [this]()
        {
            if (loadedProgram.groups <= 1)
            {
                DBG(
                    "DELETE KG TEST: CANNOT DELETE LAST KG"
                );
                return;
            }

            const int deleteIndex =
                loadedProgram.groups - 1;


            sysExSender.sendDeleteKeygroup(
                loadedProgram.programNumber,
                deleteIndex
            );

            juce::Timer::callAfterDelay(
                300,
                [this]()
                {
                    DBG(
                        "RELOAD AFTER DELETE KG"
                    );

                    loadProgram(
                        loadedProgram.programNumber
                    );
                }
            );
        };


    // ========================================
    // COMPARE
    // ========================================

    addAndMakeVisible(compareButton);

    compareButton.onClick =
        [this]()
        {
            compareLatest();
        };

#endif

    addAndMakeVisible(programCombo);

    programCombo.setTextWhenNothingSelected(
        "Select Program"
    );

    programCombo.onChange =
        [this]()
        {
            const int selectedId =
                programCombo.getSelectedId();

            if (selectedId <= 0)
                return;

            const int programIndex =
                selectedId - 1;

            sysExSender.sendProgramChange(
                programIndex,
                basicMidiChannel + 1
            );
            DBG("=== UI PROGRAM CLICK ===");

            loadProgram(
                programIndex
            );
        };




    requestRPDATAButton.onClick =
        [this]
        {
            if (currentProgram < 0)
            {
                DBG("GET PROGRAM: NO PROGRAM SELECTED");
                return;
            }

            loadProgram(
                currentProgram
            );
        };




    keygroupMap.onRangeChanged =
        [this](
            int keygroupIndex,
            int lowNote,
            int highNote)
        {
            if (keygroupIndex < 0 ||
                keygroupIndex >=
                static_cast<int>(
                    loadedProgram.keygroups.size()
                    ))
            {
                return;
            }

            auto& kg =
                loadedProgram.keygroups[
                    keygroupIndex
                ];

            kg.lowNote = lowNote;
            kg.highNote = highNote;



            // �I�𒆂�KG�Ȃ�Editor�ɂ������f
            if (currentKeygroup ==
                keygroupIndex)
            {
                keyGroupEditor.setKeygroup(
                    kg,
                    keygroupIndex
                );
            }
        };


    keygroupMap.onRangeChangeFinished =
        [this](
            int keygroupIndex,
            int lowNote,
            int highNote)
        {
            if (keygroupIndex < 0 ||
                keygroupIndex >=
                static_cast<int>(
                    loadedProgram.keygroups.size()
                    ))
            {
                return;
            }

            auto& kg =
                loadedProgram.keygroups[keygroupIndex];

            kg.lowNote = lowNote;
            kg.highNote = highNote;



            auto encoded =
                KeygroupEncoder::encode(kg);




            sysExSender.sendKeygroupData(
                loadedProgram.programNumber,
                keygroupIndex,
                encoded
            );


        };



    keyGroupEditor.onRequestKeyboardFocus =
        [this]()
        {
            juce::MessageManager::callAsync(
                [this]()
                {
                    juce::Component::unfocusAllComponents();

                    keyboardComponent.grabKeyboardFocus();

                    DBG("FORCED KEYBOARD FOCUS");
                }
            );
        };


    keyGroupEditor.onFilterFreqChanged =
        [this](int keygroupIndex, int value)
        {
            if (currentProgramIndex < 0)
                return;

            if (waitingForSampleRenameReply ||
                waitingForDeleteReply ||
                pendingAddStage != PendingAddStage::none ||
                pendingAddProgramStage != PendingAddProgramStage::none)
            {
                DBG("FILTER FREQ BLOCKED - COMMAND IN PROGRESS");
                return;
            }


            if (keyGroupEditor.isFilterEditAll())
            {
                for (auto& kg : loadedProgram.keygroups)
                {
                    kg.filter.freq = value;
                }

                DBG(
                    "FILTER FREQ ALL WRITE VALUE="
                    + juce::String(value)
                );

                if (waitingForFilterReply)
                {
                    pendingFilterFreqValue = value;
                    pendingFilterFreqKeygroup = 0x7f;

                    DBG(
                        "FILTER FREQ ALL PENDING VALUE="
                        + juce::String(value)
                    );

                    return;
                }

                waitingForFilterReply = true;

                DBG("FILTER SEND time="
                    + juce::String(juce::Time::getMillisecondCounter()));

                sysExSender.sendKeygroupByte(
                    currentProgramIndex,
                    0x7f,
                    KeygroupHeaderOffset::Filter::FILFRQ,
                    static_cast<uint8_t>(value)
                );

                return;
            }

            if (keygroupIndex < 0 ||
                keygroupIndex >= static_cast<int>(
                    loadedProgram.keygroups.size()))
            {
                return;
            }

            loadedProgram
                .keygroups[keygroupIndex]
                .filter.freq = value;

 

            if (waitingForFilterReply)
            {
                pendingFilterFreqValue = value;
                pendingFilterFreqKeygroup = keygroupIndex;

                DBG(
                    "FILTER FREQ PENDING VALUE="
                    + juce::String(value)
                );

                return;
            }

            waitingForFilterReply = true;

            DBG("FILTER SEND time="
                + juce::String(juce::Time::getMillisecondCounter()));

            sysExSender.sendKeygroupByte(
                currentProgramIndex,
                keygroupIndex,
                KeygroupHeaderOffset::Filter::FILFRQ,
                static_cast<uint8_t>(value)
            );
        };

    keyGroupEditor.onFilterKeyFollowChanged =
        [this](int keygroupIndex, int value)
        {
            if (keygroupIndex < 0 ||
                keygroupIndex >= static_cast<int>(
                    loadedProgram.keygroups.size()
                    ))
            {
                return;
            }

            loadedProgram
                .keygroups[keygroupIndex]
                .filter.keyFollow = value;



            sysExSender.sendKeygroupByte(
                currentProgramIndex,
                keygroupIndex,
                KeygroupHeaderOffset::Filter::K_FREQ,
                static_cast<uint8_t>(value)
            );
        };

    keyGroupEditor.onEnv2R1Changed =
        [this](int keygroupIndex, int value)
        {

            if (keygroupIndex < 0 ||
                keygroupIndex >= static_cast<int>(
                    loadedProgram.keygroups.size()))
                return;

            const bool editAll =
                keyGroupEditor.isEnv2EditAll();

            const int targetKeygroup =
                editAll
                ? 0x7f
                : keygroupIndex;

            // ローカル値更新
            if (editAll)
            {
                for (auto& kg : loadedProgram.keygroups)
                    kg.env2.r1 = value;

                if (waitingForFilterReply)
                {
                    pendingEnv2R1Value = value;
                    pendingEnv2R1Keygroup = 0x7f;
                    return;
                }

                waitingForFilterReply = true;



                sysExSender.sendKeygroupHeaderByte(
                    loadedProgram.programNumber,
                    0x7f,
                    static_cast<int>(
                        KeygroupHeaderOffset::Env2::R1
                        ),
                    value
                );

                return;
            }
            else
            {
                loadedProgram.keygroups[keygroupIndex]
                    .env2.r1 = value;
            }

            // ACK待ち中なら最新値だけ保持
            if (waitingForFilterReply)
            {
                pendingEnv2R1Value = value;
                pendingEnv2R1Keygroup = targetKeygroup;



                return;
            }

            waitingForFilterReply = true;

            sysExSender.sendKeygroupByte(
                currentProgramIndex,
                targetKeygroup,
                static_cast<int>(
                    KeygroupHeaderOffset::Env2::R1
                    ),
                static_cast<uint8_t>(value)
            );
        };

    keyGroupEditor.onEnv2L1Changed =
        [this](int keygroupIndex, int value)
        {



            if (keygroupIndex < 0 ||
                keygroupIndex >= static_cast<int>(
                    loadedProgram.keygroups.size()))
                return;

            const bool editAll =
                keyGroupEditor.isEnv2EditAll();

            const int targetKeygroup =
                editAll
                ? 0x7f
                : keygroupIndex;

            // ローカル値更新
            if (editAll)
            {
                for (auto& kg : loadedProgram.keygroups)
                    kg.env2.l1 = value;
            }
            else
            {
                loadedProgram.keygroups[keygroupIndex]
                    .env2.l1 = value;
            }

            // ACK待ち中なら最新値だけ保持
            if (waitingForFilterReply)
            {
                pendingEnv2L1Value = value;
                pendingEnv2L1Keygroup = targetKeygroup;


                return;
            }

            waitingForFilterReply = true;

            sysExSender.sendKeygroupHeaderByte(
                loadedProgram.programNumber,
                targetKeygroup,
                static_cast<int>(
                    KeygroupHeaderOffset::Env2::L1
                    ),
                value
            );
        };

    keyGroupEditor.onEnv2R2Changed =
        [this](int keygroupIndex, int value)
        {
            if (keygroupIndex < 0 ||
                keygroupIndex >= static_cast<int>(
                    loadedProgram.keygroups.size()))
                return;

            const bool editAll =
                keyGroupEditor.isEnv2EditAll();

            const int targetKeygroup =
                editAll ? 0x7f : keygroupIndex;

            if (editAll)
            {
                for (auto& kg : loadedProgram.keygroups)
                    kg.env2.r2 = value;
            }
            else
            {
                loadedProgram.keygroups[keygroupIndex]
                    .env2.r2 = value;
            }

            if (waitingForFilterReply)
            {
                pendingEnv2R2Value = value;
                pendingEnv2R2Keygroup = targetKeygroup;
                return;
            }

            waitingForFilterReply = true;

            sysExSender.sendKeygroupHeaderByte(
                loadedProgram.programNumber,
                targetKeygroup,
                static_cast<int>(
                    KeygroupHeaderOffset::Env2::R2
                    ),
                value
            );
        };

    keyGroupEditor.onEnv2L2Changed =
        [this](int keygroupIndex, int value)
        {
            if (keygroupIndex < 0 ||
                keygroupIndex >= static_cast<int>(
                    loadedProgram.keygroups.size()))
                return;

            const bool editAll =
                keyGroupEditor.isEnv2EditAll();

            const int targetKeygroup =
                editAll ? 0x7f : keygroupIndex;

            if (editAll)
            {
                for (auto& kg : loadedProgram.keygroups)
                    kg.env2.l2 = value;
            }
            else
            {
                loadedProgram.keygroups[keygroupIndex]
                    .env2.l2 = value;
            }

            if (waitingForFilterReply)
            {
                pendingEnv2L2Value = value;
                pendingEnv2L2Keygroup = targetKeygroup;
                return;
            }

            waitingForFilterReply = true;

            sysExSender.sendKeygroupHeaderByte(
                loadedProgram.programNumber,
                targetKeygroup,
                static_cast<int>(
                    KeygroupHeaderOffset::Env2::L2
                    ),
                value
            );
        };


    keyGroupEditor.onEnv2R3Changed =
        [this](int keygroupIndex, int value)
        {
            if (keygroupIndex < 0 ||
                keygroupIndex >= static_cast<int>(
                    loadedProgram.keygroups.size()))
                return;

            const bool editAll =
                keyGroupEditor.isEnv2EditAll();

            const int targetKeygroup =
                editAll ? 0x7f : keygroupIndex;

            if (editAll)
            {
                for (auto& kg : loadedProgram.keygroups)
                    kg.env2.r3 = value;
            }
            else
            {
                loadedProgram.keygroups[keygroupIndex]
                    .env2.r3 = value;
            }

            if (waitingForFilterReply)
            {
                pendingEnv2R3Value = value;
                pendingEnv2R3Keygroup = targetKeygroup;
                return;
            }

            waitingForFilterReply = true;

            sysExSender.sendKeygroupHeaderByte(
                loadedProgram.programNumber,
                targetKeygroup,
                static_cast<int>(
                    KeygroupHeaderOffset::Env2::R3
                    ),
                value
            );
        };

    keyGroupEditor.onEnv2L3Changed =
        [this](int keygroupIndex, int value)
        {
            if (keygroupIndex < 0 ||
                keygroupIndex >= static_cast<int>(
                    loadedProgram.keygroups.size()))
                return;

            const bool editAll =
                keyGroupEditor.isEnv2EditAll();

            const int targetKeygroup =
                editAll ? 0x7f : keygroupIndex;

            if (editAll)
            {
                for (auto& kg : loadedProgram.keygroups)
                    kg.env2.l3 = value;
            }
            else
            {
                loadedProgram.keygroups[keygroupIndex]
                    .env2.l3 = value;
            }

            if (waitingForFilterReply)
            {
                pendingEnv2L3Value = value;
                pendingEnv2L3Keygroup = targetKeygroup;
                return;
            }

            waitingForFilterReply = true;

            sysExSender.sendKeygroupHeaderByte(
                loadedProgram.programNumber,
                targetKeygroup,
                static_cast<int>(
                    KeygroupHeaderOffset::Env2::L3
                    ),
                value
            );
        };

    keyGroupEditor.onEnv2R4Changed =
        [this](int keygroupIndex, int value)
        {
            if (keygroupIndex < 0 ||
                keygroupIndex >= static_cast<int>(
                    loadedProgram.keygroups.size()))
                return;

            const bool editAll =
                keyGroupEditor.isEnv2EditAll();

            const int targetKeygroup =
                editAll ? 0x7f : keygroupIndex;

            if (editAll)
            {
                for (auto& kg : loadedProgram.keygroups)
                    kg.env2.r4 = value;
            }
            else
            {
                loadedProgram.keygroups[keygroupIndex]
                    .env2.r4 = value;
            }

            if (waitingForFilterReply)
            {
                pendingEnv2R4Value = value;
                pendingEnv2R4Keygroup = targetKeygroup;
                return;
            }

            waitingForFilterReply = true;

            sysExSender.sendKeygroupHeaderByte(
                loadedProgram.programNumber,
                targetKeygroup,
                static_cast<int>(
                    KeygroupHeaderOffset::Env2::R4
                    ),
                value
            );
        };

    keyGroupEditor.onEnv2L4Changed =
        [this](int keygroupIndex, int value)
        {
            if (keygroupIndex < 0 ||
                keygroupIndex >= static_cast<int>(
                    loadedProgram.keygroups.size()))
                return;

            const bool editAll =
                keyGroupEditor.isEnv2EditAll();

            const int targetKeygroup =
                editAll ? 0x7f : keygroupIndex;

            if (editAll)
            {
                for (auto& kg : loadedProgram.keygroups)
                    kg.env2.l4 = value;
            }
            else
            {
                loadedProgram.keygroups[keygroupIndex]
                    .env2.l4 = value;
            }

            if (waitingForFilterReply)
            {
                pendingEnv2L4Value = value;
                pendingEnv2L4Keygroup = targetKeygroup;
                return;
            }

            waitingForFilterReply = true;

            sysExSender.sendKeygroupHeaderByte(
                loadedProgram.programNumber,
                targetKeygroup,
                static_cast<int>(
                    KeygroupHeaderOffset::Env2::L4
                    ),
                value
            );
        };

    keyGroupEditor.onEnv1AttackChanged =
        [this](int keygroupIndex, int value)
        {
            if (keygroupIndex < 0 ||
                keygroupIndex >= static_cast<int>(
                    loadedProgram.keygroups.size()))
                return;

            const bool editAll =
                keyGroupEditor.isEnv1EditAll();

            const int targetKeygroup =
                editAll ? 0x7f : keygroupIndex;

            if (editAll)
            {
                for (auto& kg : loadedProgram.keygroups)
                    kg.env1.attack = value;
            }
            else
            {
                loadedProgram
                    .keygroups[keygroupIndex]
                    .env1.attack = value;
            }

            if (waitingForFilterReply)
            {
                pendingEnv1AttackValue = value;
                pendingEnv1AttackKeygroup =
                    targetKeygroup;
                return;
            }

            waitingForFilterReply = true;

            DBG(
                "ENV1 ATTACK SEND"
                " KG=" + juce::String(targetKeygroup)
                + " OFFSET="
                + juce::String(
                    static_cast<int>(
                        KeygroupHeaderOffset::Env1::ATTACK))
                + " VALUE="
                + juce::String(value)
            );

            sysExSender.sendKeygroupHeaderByte(
                loadedProgram.programNumber,
                targetKeygroup,
                static_cast<int>(
                    KeygroupHeaderOffset::Env1::ATTACK
                    ),
                value
            );
        };

    keyGroupEditor.onEnv1DecayChanged =
        [this](int keygroupIndex, int value)
        {
            if (keygroupIndex < 0 ||
                keygroupIndex >= static_cast<int>(
                    loadedProgram.keygroups.size()))
                return;

            const bool editAll =
                keyGroupEditor.isEnv1EditAll();

            const int targetKeygroup =
                editAll ? 0x7f : keygroupIndex;

            if (editAll)
            {
                for (auto& kg : loadedProgram.keygroups)
                    kg.env1.decay = value;
            }
            else
            {
                loadedProgram
                    .keygroups[keygroupIndex]
                    .env1.decay = value;
            }

            if (waitingForFilterReply)
            {
                pendingEnv1DecayValue = value;
                pendingEnv1DecayKeygroup =
                    targetKeygroup;
                return;
            }

            waitingForFilterReply = true;

            sysExSender.sendKeygroupHeaderByte(
                loadedProgram.programNumber,
                targetKeygroup,
                static_cast<int>(
                    KeygroupHeaderOffset::Env1::DECAY
                    ),
                value
            );
        };

    keyGroupEditor.onEnv1SustainChanged =
        [this](int keygroupIndex, int value)
        {
            if (keygroupIndex < 0 ||
                keygroupIndex >= static_cast<int>(
                    loadedProgram.keygroups.size()))
                return;

            const bool editAll =
                keyGroupEditor.isEnv1EditAll();

            const int targetKeygroup =
                editAll ? 0x7f : keygroupIndex;

            if (editAll)
            {
                for (auto& kg : loadedProgram.keygroups)
                    kg.env1.sustain = value;
            }
            else
            {
                loadedProgram
                    .keygroups[keygroupIndex]
                    .env1.sustain = value;
            }

            if (waitingForFilterReply)
            {
                pendingEnv1SustainValue = value;
                pendingEnv1SustainKeygroup =
                    targetKeygroup;
                return;
            }

            waitingForFilterReply = true;

            sysExSender.sendKeygroupHeaderByte(
                loadedProgram.programNumber,
                targetKeygroup,
                static_cast<int>(
                    KeygroupHeaderOffset::Env1::SUSTAIN
                    ),
                value
            );
        };

    keyGroupEditor.onEnv1ReleaseChanged =
        [this](int keygroupIndex, int value)
        {
            if (keygroupIndex < 0 ||
                keygroupIndex >= static_cast<int>(
                    loadedProgram.keygroups.size()))
                return;

            const bool editAll =
                keyGroupEditor.isEnv1EditAll();

            const int targetKeygroup =
                editAll ? 0x7f : keygroupIndex;

            if (editAll)
            {
                for (auto& kg : loadedProgram.keygroups)
                    kg.env1.release = value;
            }
            else
            {
                loadedProgram
                    .keygroups[keygroupIndex]
                    .env1.release = value;
            }

            if (waitingForFilterReply)
            {
                pendingEnv1ReleaseValue = value;
                pendingEnv1ReleaseKeygroup =
                    targetKeygroup;
                return;
            }

            waitingForFilterReply = true;

            sysExSender.sendKeygroupHeaderByte(
                loadedProgram.programNumber,
                targetKeygroup,
                static_cast<int>(
                    KeygroupHeaderOffset::Env1::RELEASE
                    ),
                value
            );
        };


    keyGroupEditor.onResonanceChanged =
        [this](int keygroupIndex, int value)
        {
            if (currentProgramIndex < 0)
                return;


            if (waitingForSampleRenameReply ||
                waitingForDeleteReply ||
                pendingAddStage != PendingAddStage::none ||
                pendingAddProgramStage != PendingAddProgramStage::none)
            {
                DBG("FILTER RESONANCE BLOCKED - COMMAND IN PROGRESS");
                return;
            }

            if (keyGroupEditor.isFilterEditAll())
            {
                for (auto& kg : loadedProgram.keygroups)
                {
                    kg.filter.resonance = value;
                }

                DBG(
                    "RESONANCE ALL WRITE VALUE="
                    + juce::String(value)
                );

                if (waitingForFilterReply)
                {
                    pendingResonanceValue = value;
                    pendingResonanceKeygroup = 0x7f;

                    DBG(
                        "FILTER RESONANCE ALL PENDING VALUE="
                        + juce::String(value)
                    );

                    return;
                }

                waitingForFilterReply = true;

                sysExSender.sendKeygroupWord(
                    currentProgramIndex,
                    0x7f,
                    KeygroupHeaderOffset::Filter::FILQ,
                    static_cast<uint16_t>(value)
                );

                return;
            }

            if (keygroupIndex < 0 ||
                keygroupIndex >= static_cast<int>(
                    loadedProgram.keygroups.size()))
            {
                return;
            }

            loadedProgram
                .keygroups[keygroupIndex]
                .filter.resonance = value;



            if (waitingForFilterReply)
            {
                pendingResonanceValue = value;
                pendingResonanceKeygroup = keygroupIndex;

                DBG(
                    "FILTER RESONANCE PENDING VALUE="
                    + juce::String(value)
                );

                return;
            }

            waitingForFilterReply = true;

            sysExSender.sendKeygroupWord(
                currentProgramIndex,
                keygroupIndex,
                KeygroupHeaderOffset::Filter::FILQ,
                static_cast<uint16_t>(value)
            );


        };

    keyGroupEditor.onKeyRangeChanged =
        [this](int low, int high)
        {
            if (currentKeygroup < 0 ||
                currentKeygroup >= static_cast<int>(
                    loadedProgram.keygroups.size()))
            {
                return;
            }

            auto& kg =
                loadedProgram.keygroups[currentKeygroup];

            const bool lowChanged =
                kg.lowNote != low;

            const bool highChanged =
                kg.highNote != high;

            kg.lowNote = low;
            kg.highNote = high;

            DBG(
                "KEY RANGE CHANGED KG="
                + juce::String(currentKeygroup)
                + " LOW="
                + juce::String(low)
                + " HIGH="
                + juce::String(high)
            );

            keygroupMap.setProgram(
                loadedProgram
            );

            programTree.updateKeygroup(
                currentKeygroup,
                kg
            );

            if (lowChanged)
            {
                DBG(
                    "LOW NOTE PARTIAL SEND KG="
                    + juce::String(currentKeygroup)
                    + " OFFSET=3 VALUE="
                    + juce::String(low)
                );

                sysExSender.sendKeygroupByte(
                    currentProgramIndex,
                    currentKeygroup,
                    3,
                    static_cast<uint8_t>(low)
                );
            }

            if (highChanged)
            {
                DBG(
                    "HIGH NOTE PARTIAL SEND KG="
                    + juce::String(currentKeygroup)
                    + " OFFSET=4 VALUE="
                    + juce::String(high)
                );

                sysExSender.sendKeygroupByte(
                    currentProgramIndex,
                    currentKeygroup,
                    4,
                    static_cast<uint8_t>(high)
                );
            }
        };



    keyGroupEditor.onKeygroupChanged =
        [this](
            int keygroupIndex,
            const Keygroup& keygroup
            )
        {
            

            if (keygroupIndex < 0 ||
                keygroupIndex >= loadedProgram.keygroups.size())
            {
                DBG("INVALID KEYGROUP INDEX");
                return;
            }

            if (currentProgramIndex < 0)
            {
                const int selectedId =
                    programCombo.getSelectedId();

                if (selectedId > 0)
                {
                    currentProgramIndex =
                        selectedId - 1;

                    DBG(
                        "RECOVERED PROGRAM INDEX FROM COMBO = "
                        + juce::String(currentProgramIndex)
                    );
                }
            }

            if (currentProgramIndex < 0)
            {
                DBG(
                    "KEYGROUP SEND CANCELLED: "
                    "NO RESIDENT PROGRAM SELECTED"
                );

                return;
            }

            const auto& previousKeygroup =
                loadedProgram.keygroups[keygroupIndex];

            const bool keyRangeChanged =
                previousKeygroup.lowNote != keygroup.lowNote
                || previousKeygroup.highNote != keygroup.highNote;

            // KeyGroupEditorはZoneを編集しないため、
// loadedProgram内の最新Zoneを保持する
            auto updatedKeygroup =
                keygroup;

            // KeyGroupEditor が編集していない最新データを保持
            updatedKeygroup.zones =
                loadedProgram
                .keygroups[keygroupIndex]
                .zones;

            updatedKeygroup.modVPitch =
                loadedProgram
                .keygroups[keygroupIndex]
                .modVPitch;

            updatedKeygroup.modFilter1 =
                loadedProgram
                .keygroups[keygroupIndex]
                .modFilter1;

            updatedKeygroup.modFilter2 =
                loadedProgram
                .keygroups[keygroupIndex]
                .modFilter2;

            updatedKeygroup.modFilter3 =
                loadedProgram
                .keygroups[keygroupIndex]
                .modFilter3;

            loadedProgram.keygroups[keygroupIndex] =
                updatedKeygroup;

            auto& storedKeygroup =
                loadedProgram.keygroups[keygroupIndex];

            auto encoded =
                KeygroupEncoder::encode(
                    storedKeygroup
                );

            if (keyRangeChanged)
            {
                DBG("KEY RANGE CHANGED - UPDATE KEYGROUP MAP");

                keygroupMap.setProgram(
                    loadedProgram
                );
            }



            if (encoded.empty())
            {
                DBG("KEYGROUP ENCODE FAILED");
                return;
            }

            


            // �����m�F
            const auto& raw =
                updatedKeygroup.rawData;




            sysExSender.sendKeygroupData(
                currentProgramIndex,
                keygroupIndex,
                encoded
            );



        };

 



    velocityZoneEditor.onSemitoneChanged =
        [this](int semitone)
        {
            if (currentKeygroup < 0 ||
                currentKeygroup >= static_cast<int>(
                    loadedProgram.keygroups.size()))
            {
                return;
            }

            if (currentZone < 0 ||
                currentZone >= 4)
            {
                return;
            }

            auto& zone =
                loadedProgram
                .keygroups[currentKeygroup]
                .zones[currentZone];

            zone.semitone = semitone;

            const int tuneOffset =
                48 + currentZone * 24;

            const int16_t fixedValue =
                static_cast<int16_t>(
                    semitone * 256
                    + zone.fineTuneRaw
                    );

            DBG(
                "SEMITONE PARTIAL SEND KG="
                + juce::String(currentKeygroup)
                + " ZONE="
                + juce::String(currentZone)
                + " OFFSET="
                + juce::String(tuneOffset)
                + " SEM="
                + juce::String(semitone)
                + " RAW="
                + juce::String(
                    static_cast<int>(fixedValue)
                )
            );

            sysExSender.sendKeygroupWord(
                currentProgramIndex,
                currentKeygroup,
                tuneOffset,
                static_cast<uint16_t>(fixedValue)
            );
        };

    velocityZoneEditor.onFineTuneChanged =
        [this](int fineTuneRaw)
        {
            if (currentKeygroup < 0 ||
                currentKeygroup >= static_cast<int>(
                    loadedProgram.keygroups.size()))
            {
                return;
            }

            if (currentZone < 0 ||
                currentZone >= 4)
            {
                return;
            }

            auto& zone =
                loadedProgram
                .keygroups[currentKeygroup]
                .zones[currentZone];

            zone.fineTuneRaw = fineTuneRaw;

            const int tuneOffset =
                48 + currentZone * 24;

            // VTUNO = semitone + binary fraction
            const int16_t fixedValue =
                static_cast<int16_t>(
                    zone.semitone * 256
                    + zone.fineTuneRaw
                    );

            DBG(
                "FINE TUNE PARTIAL SEND KG="
                + juce::String(currentKeygroup)
                + " ZONE="
                + juce::String(currentZone)
                + " OFFSET="
                + juce::String(tuneOffset)
                + " SEM="
                + juce::String(zone.semitone)
                + " FINE RAW="
                + juce::String(zone.fineTuneRaw)
                + " VALUE="
                + juce::String(
                    static_cast<int>(fixedValue)
                )
            );

            sysExSender.sendKeygroupWord(
                currentProgramIndex,
                currentKeygroup,
                tuneOffset,
                static_cast<uint16_t>(fixedValue)
            );
        };

    velocityZoneEditor.onLoudnessChanged =
        [this](int value)
        {
            if (currentKeygroup < 0 ||
                currentKeygroup >= static_cast<int>(
                    loadedProgram.keygroups.size()))
                return;

            if (currentZone < 0 || currentZone >= 4)
                return;

            loadedProgram
                .keygroups[currentKeygroup]
                .zones[currentZone]
                .loudness = value;

            const int offset =
                50 + currentZone * 24;

            DBG(
                "LOUDNESS PARTIAL SEND KG="
                + juce::String(currentKeygroup)
                + " ZONE="
                + juce::String(currentZone)
                + " OFFSET="
                + juce::String(offset)
                + " VALUE="
                + juce::String(value)
            );

            sysExSender.sendKeygroupByte(
                currentProgramIndex,
                currentKeygroup,
                offset,
                static_cast<uint8_t>(
                    static_cast<int8_t>(value)
                    )
            );
        };


    velocityZoneEditor.onFilterFreqChanged =
        [this](int value)
        {
            if (currentKeygroup < 0 ||
                currentKeygroup >= static_cast<int>(
                    loadedProgram.keygroups.size()))
                return;

            if (currentZone < 0 || currentZone >= 4)
                return;

            loadedProgram
                .keygroups[currentKeygroup]
                .zones[currentZone]
                .filterFreq = value;

            const int offset =
                51 + currentZone * 24;

            DBG(
                "FILTER FREQ PARTIAL SEND KG="
                + juce::String(currentKeygroup)
                + " ZONE="
                + juce::String(currentZone)
                + " OFFSET="
                + juce::String(offset)
                + " VALUE="
                + juce::String(value)
            );

            sysExSender.sendKeygroupByte(
                currentProgramIndex,
                currentKeygroup,
                offset,
                static_cast<uint8_t>(
                    static_cast<int8_t>(value)
                    )
            );
        };


    velocityZoneEditor.onPanChanged =
        [this](int value)
        {
            if (currentKeygroup < 0 ||
                currentKeygroup >= static_cast<int>(
                    loadedProgram.keygroups.size()))
                return;

            if (currentZone < 0 || currentZone >= 4)
                return;

            loadedProgram
                .keygroups[currentKeygroup]
                .zones[currentZone]
                .pan = value;

            const int offset =
                52 + currentZone * 24;

            DBG(
                "PAN PARTIAL SEND KG="
                + juce::String(currentKeygroup)
                + " ZONE="
                + juce::String(currentZone)
                + " OFFSET="
                + juce::String(offset)
                + " VALUE="
                + juce::String(value)
            );

            sysExSender.sendKeygroupByte(
                currentProgramIndex,
                currentKeygroup,
                offset,
                static_cast<uint8_t>(
                    static_cast<int8_t>(value)
                    )
            );
        };

    velocityZoneEditor.onZoneChanged =
        [this](const VelocityZone& zone)
        {
            DBG(
                "ZONE CHANGE:"
                " currentKeygroup="
                + juce::String(currentKeygroup)
                + " currentZone="
                + juce::String(currentZone)
                + " keygroupCount="
                + juce::String(
                    static_cast<int>(
                        loadedProgram.keygroups.size()
                        )
                )
            );

            const int keygroupCount =
                static_cast<int>(
                    loadedProgram.keygroups.size()
                    );

            if (currentKeygroup < 0
                || currentKeygroup >= keygroupCount)
            {
                DBG(
                    "ZONE CHANGE CANCELLED: "
                    "INVALID KEYGROUP INDEX"
                );

                return;
            }

            if (currentZone < 0
                || currentZone >= 4)
            {
                DBG(
                    "ZONE CHANGE CANCELLED: "
                    "INVALID ZONE INDEX"
                );

                return;
            }

            auto& kg =
                loadedProgram.keygroups[
                    currentKeygroup
                ];

            // ========================================
            // ���[�J��Zone�X�V
            // ========================================

            kg.zones[currentZone] =
                zone;

            // ========================================
            // 4-Zone Overview�����X�V
            // ========================================

            velocityZoneEditor.setZones(
                kg.zones
            );

            // �I���n�C���C�g���ێ�
            velocityZoneEditor.setSelectedZone(
                currentZone
            );

            programTree.updateZone(
                currentKeygroup,
                currentZone,
                zone
            );

            // ========================================
            // ���@��KDATA
            // ========================================

            auto encoded =
                KeygroupEncoder::encode(
                    kg
                );

            if (encoded.empty())
            {
                DBG("ZONE CHANGE: ENCODE FAILED");
                return;
            }

            sysExSender.sendKeygroupData(
                currentProgramIndex,
                currentKeygroup,
                encoded
            );

            // ========================================
// CP1〜CP4を個別送信
// 0 = TRACK, 1 = CONST
// ========================================

            /*const int constantPitchOffset =
                132 + currentZone;*/

            //const uint8_t constantPitchValue =
            //    kg.zones[currentZone].constantPitch
            //    ? 1
            //    : 0;

            //sysExSender.sendKeygroupByte(
            //    currentProgramIndex,
            //    currentKeygroup,
            //    constantPitchOffset,
            //    constantPitchValue
            //);



            // ========================================
// Update Sample Header Editor
// ========================================

            if (zone.sampleId >= 0)
            {
                auto it =
                    sampleHeaders.find(zone.sampleId);

                if (it != sampleHeaders.end())
                {
                    DBG(
                        "SAMPLE HEADER CACHE HIT ID="
                        + juce::String(zone.sampleId)
                        + " NAME=["
                        + it->second.name.trim()
                        + "]"
                    );

                    DBG(
                        "SAMPLE SLOCAT="
                        + juce::String((juce::int64)it->second.location)
                        + " SLNGTH="
                        + juce::String((juce::int64)it->second.length)
                    );

                    sampleHeaderEditor.setSampleHeader(
                        it->second
                    );
                }
                else
                {
                    DBG(
                        "SAMPLE HEADER CACHE MISS ID="
                        + juce::String(zone.sampleId)

                    );
                    sendSampleHeader(
                        zone.sampleId
                    );
                }
            }

        };;

    velocityZoneEditor.onTrackingChanged =
        [this](bool constantPitch)
        {
            if (currentKeygroup < 0 ||
                currentKeygroup >= static_cast<int>(
                    loadedProgram.keygroups.size()))
            {
                return;
            }

            if (currentZone < 0 ||
                currentZone >= 4)
            {
                return;
            }

            auto& zone =
                loadedProgram
                .keygroups[currentKeygroup]
                .zones[currentZone];

            zone.constantPitch =
                constantPitch;

            const int constantPitchOffset =
                132 + currentZone;

            const uint8_t constantPitchValue =
                constantPitch
                ? 1
                : 0;

            DBG(
                "TRACKING PARTIAL SEND KG="
                + juce::String(currentKeygroup)
                + " ZONE="
                + juce::String(currentZone)
                + " OFFSET="
                + juce::String(constantPitchOffset)
                + " VALUE="
                + juce::String(
                    constantPitchValue
                )
            );

            sysExSender.sendKeygroupByte(
                currentProgramIndex,
                currentKeygroup,
                constantPitchOffset,
                constantPitchValue
            );
        };


    juce::File a = juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("dump_A.bin");
    juce::File b = juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("dump_B.bin");

    if (a.existsAsFile() && b.existsAsFile())
    {
        compareDumps(a, b);
    }
    else
    {
        DBG("Dump files not found yet");
    }

    if (!restoreS3000MidiDevices())
    {
        DBG("S3000 MIDI SETUP REQUIRED");

        showS3000MidiSetup();
    }


    sampleHeaderEditor.onSampleHeaderChanged =
        [this](const SampleHeader& header)
        {
            // ==============================
            // Safety guard
            // ==============================

            if (header.id < 0)
            {
                DBG("REFUSE SAMPLE HEADER SEND: INVALID ID");
                return;
            }

            if (header.rawData.size() != SampleHeaderFormat::Size)
            {
                DBG(
                    "REFUSE SAMPLE HEADER SEND: INVALID RAW SIZE = "
                    + juce::String((int)header.rawData.size())
                );

                return;
            }

            if (header.length == 0)
            {
                DBG(
                    "REFUSE SAMPLE HEADER SEND: LENGTH = 0"
                );

                return;
            }

            const auto oldIt =
                sampleHeaders.find(header.id);

            const juce::String oldName =
                oldIt != sampleHeaders.end()
                ? oldIt->second.name
                : juce::String();

            const bool nameChanged =
                !oldName.isEmpty()
                && !oldName.equalsIgnoreCase(header.name);

            if (nameChanged)
            {
                const auto newName =
                    header.name
                    .trim()
                    .toUpperCase();

                for (const auto& [sampleId, existingName] : residentSamples)
                {
                    if (sampleId == header.id)
                        continue;

                    if (existingName
                        .trim()
                        .equalsIgnoreCase(newName))
                    {
                        DBG(
                            "SAMPLE RENAME REJECTED: DUPLICATE NAME = "
                            + newName
                        );

                        sampleHeaderEditor.setSampleNameWithoutNotification(
                            oldIt->second.name
                        );

                        return;
                    }
                }
            }

            DBG(
                "RENAME DEBUG oldName=["
                + oldName
                + "] newName=["
                + header.name
                + "] nameChanged="
                + juce::String(nameChanged ? "true" : "false")
            );

            if (oldIt != sampleHeaders.end())
            {
                DBG(
                    "CACHE NAME=["
                    + oldIt->second.name
                    + "]"
                );
            }
            else
            {
                DBG("CACHE HEADER NOT FOUND");
            }

            // ==============================
// Ignore no-op updates
// ==============================

            if (oldIt != sampleHeaders.end())
            {
                const auto oldEncoded =
                    SampleHeaderEncoder::encode(
                        oldIt->second
                    );

                const auto newEncoded =
                    SampleHeaderEncoder::encode(
                        header
                    );

                if (oldEncoded == newEncoded)
                {
                    DBG(
                        "IGNORE SAMPLE HEADER SEND: NO DATA CHANGE"
                    );

                    return;
                }
            }


            DBG("SAMPLE HEADER CHANGED");

            DBG(
                "ID = "
                + juce::String(header.id)
            );

            DBG(
                "NAME = "
                + header.name
            );

            DBG(
                "LENGTH = "
                + juce::String(
                    (juce::int64)header.length
                )
            );

            DBG(
                "TUNE = "
                + juce::String(header.tune, 2)
            );

            // ���[�J���f�[�^�X�V
            //sampleHeaders[header.id] = header;

            std::vector<uint8_t> sysex;

            if (nameChanged)
            {
                if (waitingForFilterReply)
                {
                    DBG(
                        "SAMPLE RENAME BLOCKED - FILTER COMMAND IN PROGRESS"
                    );

                    return;
                }

                DBG("SEND SAMPLE NAME ONLY");

                waitingForSampleRenameReply = true;
                pendingRenamedSampleId = header.id;

                sysex =
                    SampleHeaderEncoder::makeNameSysEx(
                        header
                    );
            }
            else
            {
                sysex =
                    SampleHeaderEncoder::makeSysEx(
                        header
                    );
            }

            DBG(
                "SYSEX SIZE = "
                + juce::String(
                    (int)sysex.size()
                )
            );

            sysExSender.sendSysEx(sysex);
            DBG("SAMPLE HEADER SYSEX SENT");

            if (!nameChanged)
            {
                sampleHeaders[header.id] = header;
            }
        };

    keyboardState.addListener(this);
    addAndMakeVisible(keyboardComponent);

    keyboardComponent.setWantsKeyboardFocus(true);

    addAndMakeVisible(keyboardToggle);

    keyboardToggle.setToggleState(
        true,
        juce::dontSendNotification
    );

    keyboardToggle.onClick = [this]()
        {
            const bool showKeyboard =
                keyboardToggle.getToggleState();

            keyboardComponent.setVisible(showKeyboard);

            keyboardVelocityLabel.setVisible(showKeyboard);
            keyboardVelocitySlider.setVisible(showKeyboard);
            keyboardOctaveCombo.setVisible(showKeyboard);

            if (showKeyboard)
                keyboardComponent.grabKeyboardFocus();

            resized();
            repaint();
        };

    addAndMakeVisible(keyboardVelocityLabel);
    keyboardVelocityLabel.setText(
        "Velocity",
        juce::dontSendNotification
    );

    addAndMakeVisible(keyboardVelocitySlider);
    keyboardVelocitySlider.setRange(1, 127, 1);
    
    keyboardVelocitySlider.setScrollWheelEnabled(false);
    
    keyboardVelocitySlider.setValue(100);
    keyboardVelocitySlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );
    keyboardVelocitySlider.setTextBoxStyle(
        juce::Slider::TextBoxRight,
        false,
        45,
        20
    );

    //addAndMakeVisible(keyboardOctaveLabel);
    //keyboardOctaveLabel.setText(
    //    "Octave",
    //    juce::dontSendNotification
    //);

    addAndMakeVisible(keyboardOctaveCombo);

    keyboardOctaveCombo.addItem("C1", 1);
    keyboardOctaveCombo.addItem("C2", 2);
    keyboardOctaveCombo.addItem("C3", 3);
    keyboardOctaveCombo.addItem("C4", 4);
    keyboardOctaveCombo.addItem("C5", 5);

    keyboardOctaveCombo.setSelectedId(
        3,
        juce::dontSendNotification
    );

    keyboardComponent.setKeyPressBaseOctave(5);

    keyboardOctaveCombo.onChange = [this]()
        {
            const int octave =
                keyboardOctaveCombo.getSelectedId();

            keyboardComponent.setKeyPressBaseOctave(
                octave + 2
            );

            DBG(
                "KEYBOARD BASE OCTAVE = "
                + juce::String(octave)
            );
        };

    addAndMakeVisible(deleteProgramButton);
    //addAndMakeVisible(addProgramButton);


    deleteProgramButton.onClick =
        [this]()
        {
            deleteProgram();
        };


    addProgramButton.onClick =
        [this]()
        {
            addProgram();
        };
    
    injectTestKeygroupHeaderByte(
        0,   // program
        0,   // keygroup
        20,  // ENV2 R1
        50
    );

    setWantsKeyboardFocus(true);
    addKeyListener(this);



    deviceStatusLabel.toFront(false);
    saveProjectButton.toFront(false);
    loadProjectButton.toFront(false);
    startTimer(1000);

}

MainComponent::~MainComponent()
{
    setLookAndFeel(nullptr);
}



void MainComponent::deleteProgram()
{
    const int programNumber =
        loadedProgram.programNumber;

    const juce::String programName =
        juce::String(loadedProgram.name);

    juce::AlertWindow::showOkCancelBox(
        juce::AlertWindow::WarningIcon,
        "Delete Program?",
        "Delete \"" + programName + "\"?\n\n"
        "This will delete the program and all of its keygroups.",
        "Delete",
        "Cancel",
        nullptr,
        juce::ModalCallbackFunction::create(
            [this, programNumber](int result)
            {
                if (result == 0)
                    return;

                sysExSender.sendDeleteProgram(
                    programNumber
                );

                sysExSender.sendRPLIST();
            }
        )
    );
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(
        getLookAndFeel().findColour(
            juce::ResizableWindow::backgroundColourId
        )
    );

    const auto akaiRed =
        juce::Colour::fromRGB(190, 45, 38);

    const auto darkText =
        juce::Colour::fromRGB(55, 55, 52);

    // =========================
    // AKAI
    // =========================

    g.setColour(akaiRed);

    g.setFont(
        juce::Font(24.0f).boldened()
    );

    g.drawText(
        "AKAI",
        20,
        31,
        65,
        28,
        juce::Justification::centredLeft
    );

    // =========================
    // Divider
    // =========================

    g.setColour(
        juce::Colour::fromRGB(130, 130, 125)
    );

    g.fillRect(
        92,
        32,
        1,
        34
    );

    // =========================
    // S3000XL
    // =========================

    g.setColour(akaiRed);

    g.setFont(
        juce::Font(20.0f).boldened()
    );

    g.drawText(
        "S3000XL",
        104,
        31,
        105,
        24,
        juce::Justification::centredLeft
    );

    // =========================
    // Subtitle
    // =========================

    g.setColour(darkText);

    g.setFont(
        juce::Font(9.0f).boldened()
    );

    g.drawText(
        "SYSEX EDITOR",
        105,
        53,
        105,
        12,
        juce::Justification::centredLeft
    );
}


juce::StringArray MainComponent::getMenuBarNames()
{
    return {
        "Program",
        "MIDI"
    };
}

juce::PopupMenu MainComponent::getMenuForIndex(
    int topLevelMenuIndex,
    const juce::String& menuName
)
{
    juce::PopupMenu menu;

    if (topLevelMenuIndex == 0)
    {
        menu.addItem(
            1001,
            "New Program"
        );

        menu.addItem(
            1002,
            "Delete Program"
        );

        menu.addItem(1005, "Reload from S3000XL");

        menu.addSeparator();

        menu.addItem(
            1003,
            "Load Program..."
        );

        menu.addItem(
            1004,
            "Save Program..."
        );
    }
    else if (topLevelMenuIndex == 1)
    {
        juce::PopupMenu inputMenu;
        juce::PopupMenu outputMenu;

        // ==============================
        // MIDI INPUT
        // ==============================

        for (int i = 0;
            i < keyboardMidiInputDevices.size();
            ++i)
        {
            inputMenu.addItem(
                2000 + i,
                keyboardMidiInputDevices[i].name
            );
        }

        // ==============================
        // MIDI OUTPUT
        // ==============================

        const auto outputs =
            juce::MidiOutput::getAvailableDevices();

        for (int i = 0;
            i < outputs.size();
            ++i)
        {
            outputMenu.addItem(
                3000 + i,
                outputs[i].name
            );
        }

        menu.addSubMenu(
            "MIDI Input",
            inputMenu
        );

        menu.addSubMenu(
            "MIDI Output",
            outputMenu
        );
    }

    return menu;
}

//void MainComponent::menuItemSelected(
//    int menuItemID,
//    int topLevelMenuIndex
//)
//{
//    DBG(
//        "MENU SELECTED ID="
//        + juce::String(menuItemID)
//        + " TOP="
//        + juce::String(topLevelMenuIndex)
//    );
//}

void MainComponent::openKeyboardMidiInput(int index)
{
    if (index < 0 ||
        index >= keyboardMidiInputDevices.size())
    {
        return;
    }

    if (keyboardMidiInput)
    {
        keyboardMidiInput->stop();
        keyboardMidiInput.reset();
    }

    const auto& device =
        keyboardMidiInputDevices[index];

    DBG(
        "OPEN KEYBOARD MIDI INPUT: "
        + device.name
    );

    keyboardMidiInput =
        juce::MidiInput::openDevice(
            device.identifier,
            this
        );

    if (keyboardMidiInput)
    {
        keyboardMidiInput->start();

        DBG(
            "KEYBOARD MIDI INPUT STARTED"
        );
    }
    else
    {
        DBG(
            "FAILED TO OPEN KEYBOARD MIDI INPUT"
        );
    }
}


void MainComponent::addProgram()
{
    if (waitingForFilterReply)
    {
        DBG("ADD PROGRAM BLOCKED - FILTER COMMAND IN PROGRESS");
        return;
    }

    if (pendingAddProgramStage !=
        PendingAddProgramStage::none)
    {
        return;
    }

    if (loadedProgram.keygroups.empty())
    {
        DBG("ADD PROGRAM: NO KEYGROUP TEMPLATE");
        return;
    }

    const int newProgramNumber =
        static_cast<int>(programList.size());

    auto newProgram =
        loadedProgram;

    newProgram.programNumber =
        newProgramNumber;

    newProgram.groups = 1;
    newProgram.keygroups.clear();

    auto newKeygroup =
        loadedProgram.keygroups[0];

    newProgram.keygroups.push_back(
        newKeygroup
    );

    newProgram.name =
        (
            juce::String("NEW PRG ")
            + juce::String(newProgramNumber)
            ).toStdString();

    auto encodedProgram =
        ProgramEncoder::encode(
            newProgram
        );

    if (encodedProgram.empty())
    {
        DBG("ADD PROGRAM: ENCODE FAILED");
        return;
    }

    std::string decodedName;

    for (std::size_t i = 0;
        i < ProgramOffset::General::NameLength;
        ++i)
    {
        const auto raw =
            encodedProgram[
                ProgramOffset::General::Name + i
            ];

        DBG(
            juce::String((int)i)
            + ": "
            + juce::String::formatted(
                "%02X",
                (unsigned)raw
            )
        );

        decodedName += decodePlistChar(raw);
    }

    while (!decodedName.empty()
        && decodedName.back() == ' ')
    {
        decodedName.pop_back();
    }

    pendingNewProgramNumber =
        newProgramNumber;

    pendingNewProgram =
        newProgram;

    pendingNewProgramKeygroup =
        newKeygroup;

    pendingAddProgramStage =
        PendingAddProgramStage::
        waitingForProgramReply;

    DBG(
        "ADD PROGRAM: SEND PDATA PROGRAM="
        + juce::String(newProgramNumber)
    );
    DBG("PDATA CALLER 3778");
    sysExSender.sendProgramData(
        newProgramNumber,
        encodedProgram
    );
}

void MainComponent::menuItemSelected(
    int menuItemID,
    int topLevelMenuIndex
)
{
    DBG(
        "MENU SELECTED ID="
        + juce::String(menuItemID)
    );

    // ==============================
    // MIDI Keyboard Input
    // ==============================
   // MIDI INPUT
    if (menuItemID >= 2000 &&
        menuItemID < 3000)
    {
        const int index =
            menuItemID - 2000;

        openKeyboardMidiInput(index);
        return;
    }

    // MIDI OUTPUT
    if (menuItemID >= 3000 &&
        menuItemID < 4000)
    {
        const int index =
            menuItemID - 3000;

        if (midiManager.openOutput(index))
        {
            sysExSender.setMidiOutput(
                midiManager.getOutput()
            );
        }

        return;
    }

    // ==============================
    // Program Menu
    // ==============================
    switch (menuItemID)
    {
    case 1001:
        addProgram();
        break;

    case 1002:
        deleteProgram();
        break;

    case 1003:
        DBG("MENU: LOAD PROJECT");
        loadProject();
        break;

    case 1004:
        DBG("MENU: SAVE PROJECT");
        saveProject();
        break;


    case 1005:
    {
        DBG("MENU: RELOAD PROGRAM FROM S3000XL");

        programRefreshPending = false;
        programRefreshOnly = false;

        loadProgram(
            loadedProgram.programNumber
        );

        break;
    }

    default:
        DBG("MENU: UNKNOWN ID");
        break;
    }
}


void MainComponent::resized()
{
    auto area = getLocalBounds();

    // =========================
// Menu bar
// =========================
    menuBar.setBounds(
        area.removeFromTop(24)
    );


    // =========================
    // Top bar
    // =========================
    auto topBar = area.removeFromTop(60);

    //auto programArea =
    //    topBar.removeFromLeft(330)
    //    .reduced(20, 15);

    //programCombo.setBounds(
    //    programArea
    //);

    topBar.removeFromLeft(230);

    auto programArea =
        topBar.removeFromLeft(330)
        .reduced(10, 15);

    programCombo.setBounds(
        programArea
    );




    deviceStatusLabel.setBounds(
        topBar.removeFromRight(250)
        .reduced(10, 15)
    );



    // =========================
// Audition keyboard
// =========================

    if (keyboardToggle.getToggleState())
    {
        // Controls + keyboard
        auto keyboardSection =
            area.removeFromBottom(120);

        auto keyboardControls =
            keyboardSection.removeFromTop(30);

        keyboardControls.removeFromLeft(20);

        keyboardToggle.setBounds(
            keyboardControls.removeFromLeft(120)
        );

        keyboardVelocityLabel.setBounds(
            keyboardControls.removeFromLeft(65)
        );

        keyboardVelocitySlider.setBounds(
            keyboardControls.removeFromLeft(150)
        );

        keyboardControls.removeFromLeft(20);

        //keyboardOctaveLabel.setBounds(
        //    keyboardControls.removeFromLeft(55)
        //);

        keyboardOctaveCombo.setBounds(
            keyboardControls.removeFromLeft(80)
       );

        keyboardComponent.setBounds(
            keyboardSection.reduced(20, 5)
        );
    }
    else
    {
        // Collapsed: only the Keyboard toggle remains
        auto keyboardControls =
            area.removeFromBottom(30);

        keyboardControls.removeFromLeft(20);

        keyboardToggle.setBounds(
            keyboardControls.removeFromLeft(120)
        );

        // Clear old bounds as well
        keyboardComponent.setBounds({});
        keyboardVelocityLabel.setBounds({});
        keyboardVelocitySlider.setBounds({});
        //keyboardOctaveLabel.setBounds({});
        keyboardOctaveCombo.setBounds({});
    }


    auto left = area.removeFromLeft(350);

    auto keygroupSection =
        left.removeFromTop(220)
        .reduced(10);


    auto keygroupHeader =
        keygroupSection.removeFromTop(34);

    keygroupTitleLabel.setBounds(
        keygroupHeader.removeFromLeft(100)
    );

    deleteKeygroupButton.setBounds(
        keygroupHeader.removeFromRight(90)
    );

    keygroupHeader.removeFromRight(6);

    addKeygroupButton.setBounds(
        keygroupHeader.removeFromRight(90)
    );

    keygroupMapViewport.setBounds(
        keygroupSection
    );

    left.removeFromTop(6);

    programTree.setBounds(
        left.reduced(10)
    );

    editorTabs.setBounds(area);


    keyGroupEditor.setSize(
        juce::jmax(
            300,
            keyGroupViewport.getWidth() - 15
        ),
        900
    );


    sampleHeaderEditor.setSize(
        juce::jmax(
            300,
            sampleHeaderViewport.getWidth() - 15
        ),
        750
    );


    programEditor.setSize(
        juce::jmax(
            300,
            programViewport.getWidth() - 15
        ),
        1250
    );

    DBG("MainComponent programEditor="
        + juce::String::toHexString(
            reinterpret_cast<juce::pointer_sized_int>(&programEditor)));

    DBG("Viewport viewedComponent="
        + juce::String::toHexString(
            reinterpret_cast<juce::pointer_sized_int>(
                programViewport.getViewedComponent())));


}

void MainComponent::compareLatest()
{
    auto desktop = juce::File::getSpecialLocation(
        juce::File::userDesktopDirectory);

    juce::File a = desktop.getChildFile("dump_A.bin");
    juce::File b = desktop.getChildFile("dump_B.bin");

    if (a.existsAsFile() && b.existsAsFile())
    {
        compareDumps(a, b);
    }
    else
    {
        DBG("Missing dump files");
    }
}

void MainComponent::timerCallback()
{
    const double now =
        juce::Time::getMillisecondCounterHiRes();

    ++heartbeatCounter;

    if (heartbeatCounter >= 1)
    {
        heartbeatCounter = 0;
        sysExSender.sendHeartbeat();
    }

    if (!deviceConnected)
        return;

    const double elapsed =
        now - lastDeviceResponseTime;

    if (elapsed > 5000.0)
    {
        DBG(
                "DEVICE TIMEOUT elapsed="
                + juce::String(elapsed)
            );
        
        setDeviceConnected(false);
    }

    if (programRefreshPending)
    {
        const auto now =
            juce::Time::getMillisecondCounterHiRes();

        if (now - lastShortProgramChangeTime > 150.0)
        {
            DBG("TIMER: SET programRefreshOnly = true");
            programRefreshPending = false;
            programRefreshOnly = true;

            DBG(
                "TIMER: BEFORE sendProgramHeader programRefreshOnly="
                + juce::String(programRefreshOnly ? 1 : 0)
            );

            sysExSender.sendProgramHeader(
                loadedProgram.programNumber
            );
        }
    }



}

void MainComponent::setDeviceConnected(bool connected)
{
    if (deviceConnected == connected)
        return;

    const bool wasConnected = deviceConnected;

    deviceConnected = connected;

    if (connected)
    {
        deviceStatusLabel.setText(
            "S3000XL: Connected",
            juce::dontSendNotification
        );

        deviceStatusLabel.setColour(
            juce::Label::textColourId,
            juce::Colour::fromRGB(55, 55, 52)
        );

        if (!wasConnected)
        {
            if (hasConnectedOnce)
            {
                DBG(
                    "DEVICE RECONNECTED - "
                    "WAITING BEFORE PROGRAM LIST"
                );

                juce::Timer::callAfterDelay(
                    5000,
                    [this]()
                    {
                        DBG(
                            "DEVICE RECONNECTED - "
                            "REQUESTING PROGRAM LIST"
                        );

                        sysExSender.sendRPLIST();
                    }
                );
            }
            else
            {
                // �A�v���N����̍ŏ��̐ڑ��m�F
                // �N������ AUTO RPLIST �����łɑ����Ă���
                DBG(
                    "DEVICE CONNECTED - "
                    "FIRST CONNECTION"
                );

                hasConnectedOnce = true;
            }
        }
    }
    else
    {
        deviceStatusLabel.setText(
            "S3000XL: Disconnected",
            juce::dontSendNotification
        );

        deviceStatusLabel.setColour(
            juce::Label::textColourId,
            juce::Colours::grey
        );
    }
}



void MainComponent::handleIncomingMidiMessage(
    juce::MidiInput* source,
    const juce::MidiMessage& message)
{

    // ========================================
    // MIDI KEYBOARD -> S3000XL THRU
    // ========================================

    if (source == keyboardMidiInput.get())
    {
        if (message.isNoteOn())
        {
            const int channel =
                message.getChannel();

            const int note =
                message.getNoteNumber();

            const int velocity =
                message.getVelocity();

            DBG(
                "PHYSICAL NOTE ON"
                " CH=" + juce::String(channel)
                + " NOTE=" + juce::String(note)
                + " VEL=" + juce::String(velocity)
            );

            sysExSender.sendNoteOn(
                note,
                velocity,
                channel
            );

            return;
        }

        if (message.isNoteOff())
        {
            const int channel =
                message.getChannel();

            const int note =
                message.getNoteNumber();

            DBG(
                "PHYSICAL NOTE OFF"
                " CH=" + juce::String(channel)
                + " NOTE=" + juce::String(note)
            );

            sysExSender.sendNoteOff(
                channel,
                note
            );

            return;
        }

        // CC / Pitch Bend / Aftertouch は従来通りそのままTHRU
        if (message.isController() ||
            message.isPitchWheel() ||
            message.isChannelPressure() ||
            message.isAftertouch())
        {
            sysExSender.sendMidiMessage(message);
        }

        return;
    }

    // ========================================
// S3000XL -> EDITOR
// ========================================




    if (message.isProgramChange())
    {
        DBG("MIDI PROGRAM CHANGE = "
            + juce::String(
                message.getProgramChangeNumber()));

        return;
    }

    if (!message.isSysEx())
        return;

    processIncomingSysEx(message);
}

void MainComponent::processIncomingSysEx(
    const juce::MidiMessage& message)
{

    auto* data = message.getSysExData();
    auto size = message.getSysExDataSize();




    if (size < 4)
        return;

    if (data[0] != 0x47 || data[1] != 0x00)
        return;

    uint8_t opcode = data[2];

    DBG(
        "RX SYSEX OPCODE=0x"
        + juce::String::toHexString((int)opcode)
        + " SIZE="
        + juce::String((int)size)

    );

    // MDATA response = heartbeat response
    if (opcode == 0x11)
    {
        const double now =
                juce::Time::getMillisecondCounterHiRes();

            DBG(
                "HEARTBEAT RESPONSE gap="
                + juce::String(
                    now - lastDeviceResponseTime
                )
            );
        
        lastDeviceResponseTime =
            juce::Time::getMillisecondCounterHiRes();

        if (size >= 12)
        {

            const int bmchan =
                (data[4] & 0x0F)
                | ((data[5] & 0x0F) << 4);

            const int selectedProgram =
                (data[10] & 0x0F)
                | ((data[11] & 0x0F) << 4);

            DBG(
                "PHYSICAL SELECTED PROGRAM NUMBER = "
                + juce::String(selectedProgram)
            );

            juce::MessageManager::callAsync(
                [this, selectedProgram, bmchan]()
                {
                    setDeviceConnected(true);

                    basicMidiChannel = bmchan;


                    DBG(
                        "MDATA ASYNC CHECK selected="
                        + juce::String(selectedProgram)
                        + " current="
                        + juce::String(currentProgram)
                    );

                    if (selectedProgram != currentProgram)
                    {
                        DBG(
                            "MDATA TRIGGER LOAD PROGRAM "
                            + juce::String(currentProgram)
                            + " -> "
                            + juce::String(selectedProgram)
                        );

                        loadProgram(selectedProgram);
                    }
                }
            );
        }
        else
        {
            juce::MessageManager::callAsync(
                [this]()
                {
                    setDeviceConnected(true);
                }
            );
        }

        return;
    }



    programBuffer.reset();
    programBuffer.append(data, size);

    static int captureIndex = 0;



    switch (opcode)
    {
        case 0x05:
        {
            handleSampleListResponse();
            break;
        }



        case 0x16:
            handleCommandReply(data, size);
            break;


        case 0x28:
        {
            DBG(
                "RECEIVED 0x28 size="
                + juce::String((int)size)
                + " refreshPending="
                + juce::String(programRefreshPending ? 1 : 0)
                + " refreshOnly="
                + juce::String(programRefreshOnly ? 1 : 0)
            );


            if (size >= 6)
            {
                const int responseProgramNumber =
                    (data[4] & 0x7F)
                    | ((data[5] & 0x7F) << 7);


            }

            // ���@�̃p�����[�^�ύX / Program�ύX�ʒm
            if (size < 395)
            {


                for (size_t i = 0; i < size; ++i)
                {
                    DBG("SHORT[" + juce::String((int)i) + "] = 0x"
                        + juce::String::toHexString((int)data[i]));
                }

                // Short Program Header byte update
                if (size == 13)
                {
                    const int offset =
                        (data[7] & 0x7F)
                        | ((data[8] & 0x7F) << 7);

                    const uint8_t rawValue =
                        (data[11] & 0x0F)
                        | ((data[12] & 0x0F) << 4);

                    const int value = static_cast<int>(rawValue);

                    DBG(
                        "SHORT PROGRAM HEADER OFFSET="
                        + juce::String(offset)
                        + " VALUE="
                        + juce::String(value)
                    );

                    if (offset == ProgramOffset::Mod::ModSFilter1)
                    {
                        loadedProgram.modSFilter1 = value;

                        DBG("UPDATE PROGRAM MODSFILTER1 VALUE="
                            + juce::String(value));
                    }
                    else if (offset == ProgramOffset::Mod::ModSFilter2)
                    {
                        loadedProgram.modSFilter2 = value;

                        DBG("UPDATE PROGRAM MODSFILTER2 VALUE="
                            + juce::String(value));
                    }
                    else if (offset == ProgramOffset::Mod::ModSFilter3)
                    {
                        loadedProgram.modSFilter3 = value;

                        DBG("UPDATE PROGRAM MODSFILTER3 VALUE="
                            + juce::String(value));
                    }
                }

                lastShortProgramChangeTime =
                    juce::Time::getMillisecondCounterHiRes();

                programRefreshPending = true;
                break;
            }

            handleProgramHeaderResponse();
            break;
        }

        case 0x2A:
        {
            if (size == 13)
            {
                const int programNumber =
                    (data[4] & 0x7F)
                    | ((data[5] & 0x7F) << 7);

                const int keygroup =
                    data[6] & 0x7F;

                const int offset =
                    (data[7] & 0x7F)
                    | ((data[8] & 0x7F) << 7);

                const int length =
                    (data[9] & 0x7F)
                    | ((data[10] & 0x7F) << 7);

                const uint8_t rawValue =
                    (data[11] & 0x0F)
                    | ((data[12] & 0x0F) << 4);

                const int signedValue =
                    static_cast<int8_t>(rawValue);

                DBG(
                    "SHORT 0x2A"
                    + juce::String(" PROGRAM=")
                    + juce::String(programNumber)
                    + " KG="
                    + juce::String(keygroup)
                    + " OFFSET="
                    + juce::String(offset)
                    + " LENGTH="
                    + juce::String(length)
                    + " RAW="
                    + juce::String((int)rawValue)
                    + " VALUE="
                    + juce::String(signedValue)
                );

                if (offset == KeygroupHeaderOffset::Mod::Lfo1Pitch
                    && keygroup >= 0
                    && keygroup < static_cast<int>(loadedProgram.keygroups.size()))
                {
                    loadedProgram.keygroups[keygroup].lfo1Pitch =
                        signedValue;

                    DBG(
                        "UPDATE KG LFO1PITCH KG="
                        + juce::String(keygroup)
                        + " VALUE="
                        + juce::String(signedValue)
                    );

                    if (keygroup == currentKeygroup)
                    {
                        juce::MessageManager::callAsync(
                            [this, signedValue]()
                            {
                                programEditor.setLfo1PitchAmount(
                                    signedValue
                                );
                            }
                        );
                    }

                    // Lfo1Pitch reply received -> request Filter1 next
                    if (extraKgParamRequest ==
                        ExtraKgParamRequest::lfo1Pitch
                        && programNumber == extraKgParamProgram
                        && keygroup == extraKgParamKeygroup)
                    {
                        DBG(
                            "EXTRA KG PARAM LFO1PITCH RECEIVED"
                            " -> NEXT FILTER1"
                        );

                        extraKgParamRequest =
                            ExtraKgParamRequest::filter1;

                        sendNextExtraKgParamRequest();
                    }

                }

                // ========================================
// Velocity Zone Output
// VLOUD / VFREQ / VPANO
// Zone stride = 24 bytes
// ========================================

                for (int zoneIndex = 0; zoneIndex < 4; ++zoneIndex)
                {

                    const int lowVelOffset =
                        46 + zoneIndex * 24;

                    const int highVelOffset =
                        47 + zoneIndex * 24;

                    const int loudnessOffset =
                        50 + zoneIndex * 24;

                    const int filterFreqOffset =
                        51 + zoneIndex * 24;

                    const int panOffset =
                        52 + zoneIndex * 24;

                    if (keygroup < 0 ||
                        keygroup >= static_cast<int>(
                            loadedProgram.keygroups.size()))
                    {
                        break;
                    }

                    auto& zone =
                        loadedProgram
                        .keygroups[keygroup]
                        .zones[zoneIndex];

                    if (offset == lowVelOffset)
                    {
                        zone.lowVel = rawValue;

                        DBG(
                            "UPDATE ZONE LOW VELOCITY KG="
                            + juce::String(keygroup)
                            + " ZONE="
                            + juce::String(zoneIndex)
                            + " VALUE="
                            + juce::String(rawValue)
                        );

                        if (keygroup == currentKeygroup)
                        {
                            juce::MessageManager::callAsync(
                                [this, keygroup, zoneIndex, rawValue]()
                                {
                                    // Zone 1-4 map 全体を更新
                                    velocityZoneEditor.setZones(
                                        loadedProgram
                                        .keygroups[keygroup]
                                        .zones
                                    );

                                    // 現在選択中Zoneなら上側Editorも更新
                                    if (zoneIndex == currentZone)
                                    {
                                        velocityZoneEditor.setLowVelocity(
                                            rawValue
                                        );
                                    }
                                }
                            );
                        }

                        break;
                    }

                    if (offset == highVelOffset)
                    {
                        zone.highVel = rawValue;

                        DBG(
                            "UPDATE ZONE HIGH VELOCITY KG="
                            + juce::String(keygroup)
                            + " ZONE="
                            + juce::String(zoneIndex)
                            + " VALUE="
                            + juce::String(rawValue)
                        );

                        if (keygroup == currentKeygroup)
                        {
                            juce::MessageManager::callAsync(
                                [this, keygroup, zoneIndex, rawValue]()
                                {
                                    velocityZoneEditor.setZones(
                                        loadedProgram
                                        .keygroups[keygroup]
                                        .zones
                                    );

                                    if (zoneIndex == currentZone)
                                    {
                                        velocityZoneEditor.setHighVelocity(
                                            rawValue
                                        );
                                    }
                                }
                            );
                        }

                        break;
                    }


                    if (offset == loudnessOffset)
                    {
                        zone.loudness = signedValue;

                        DBG(
                            "UPDATE ZONE LOUDNESS KG="
                            + juce::String(keygroup)
                            + " ZONE="
                            + juce::String(zoneIndex)
                            + " VALUE="
                            + juce::String(signedValue)
                        );

                        if (keygroup == currentKeygroup &&
                            zoneIndex == currentZone)
                        {
                            juce::MessageManager::callAsync(
                                [this, signedValue]()
                                {
                                    velocityZoneEditor.setLoudness(
                                        signedValue
                                    );
                                }
                            );
                        }

                        break;
                    }

                    if (offset == filterFreqOffset)
                    {
                        zone.filterFreq = signedValue;

                        DBG(
                            "UPDATE ZONE FILTER FREQ KG="
                            + juce::String(keygroup)
                            + " ZONE="
                            + juce::String(zoneIndex)
                            + " VALUE="
                            + juce::String(signedValue)
                        );

                        if (keygroup == currentKeygroup &&
                            zoneIndex == currentZone)
                        {
                            juce::MessageManager::callAsync(
                                [this, signedValue]()
                                {
                                    velocityZoneEditor.setFilterFreq(
                                        signedValue
                                    );
                                }
                            );
                        }

                        break;
                    }

                    if (offset == panOffset)
                    {
                        zone.pan = signedValue;

                        DBG(
                            "UPDATE ZONE PAN KG="
                            + juce::String(keygroup)
                            + " ZONE="
                            + juce::String(zoneIndex)
                            + " VALUE="
                            + juce::String(signedValue)
                        );

                        if (keygroup == currentKeygroup &&
                            zoneIndex == currentZone)
                        {
                            juce::MessageManager::callAsync(
                                [this, signedValue]()
                                {
                                    velocityZoneEditor.setPan(
                                        signedValue
                                    );
                                }
                            );
                        }

                        break;
                    }
                }

                if (offset == KeygroupHeaderOffset::Filter::K_FREQ
                    && keygroup >= 0
                    && keygroup < static_cast<int>(loadedProgram.keygroups.size()))
                {
                    loadedProgram.keygroups[keygroup].filter.keyFollow =
                        static_cast<int>(rawValue);

                    DBG(
                        "UPDATE KG FILTER KEY FOLLOW KG="
                        + juce::String(keygroup)
                        + " VALUE="
                        + juce::String((int)rawValue)
                    );

                    if (keygroup == currentKeygroup)
                    {
                        juce::MessageManager::callAsync(
                            [this, rawValue]()
                            {
                                keyGroupEditor.setFilterKeyFollow(
                                    static_cast<int>(rawValue)
                                );
                            }
                        );
                    }
                }
                
                else if (offset == KeygroupHeaderOffset::Env2::R1)
                {
                    DBG("ENV2 R1 RECEIVED = " + juce::String(rawValue));

                    DBG(
                        "ENV2 R1 RECEIVE CHECK"
                        " KG=" + juce::String(keygroup)
                        + " KEYGROUP COUNT="
                        + juce::String(
                            static_cast<int>(loadedProgram.keygroups.size())
                        )
                    );

                    if (keygroup >= 0 &&
                        keygroup < static_cast<int>(loadedProgram.keygroups.size()))
                    {
                        loadedProgram.keygroups[keygroup].env2.r1 =
                            static_cast<int>(rawValue);

                        juce::MessageManager::callAsync(
                            [this, rawValue]()
                            {
                                keyGroupEditor.setEnv2R1(
                                    static_cast<int>(rawValue)
                                );
                            }
                        );
                    }
                }
                else if (offset == KeygroupHeaderOffset::Env2::L1)
                {
                    DBG("ENV2 L1 RECEIVED = " + juce::String(rawValue));

                    if (keygroup >= 0 &&
                        keygroup < static_cast<int>(loadedProgram.keygroups.size()))
                    {
                        loadedProgram.keygroups[keygroup].env2.l1 =
                            static_cast<int>(rawValue);

                        juce::MessageManager::callAsync(
                            [this, rawValue]()
                            {
                                keyGroupEditor.setEnv2L1(
                                    static_cast<int>(rawValue)
                                );
                            }
                        );
                    }
                }
                else if (offset == KeygroupHeaderOffset::Env2::R2)
                {
                    DBG("ENV2 R2 RECEIVED = " + juce::String(rawValue));

                    if (keygroup >= 0 &&
                        keygroup < static_cast<int>(loadedProgram.keygroups.size()))
                    {
                        loadedProgram.keygroups[keygroup].env2.r2 =
                            static_cast<int>(rawValue);

                        juce::MessageManager::callAsync(
                            [this, rawValue]()
                            {
                                keyGroupEditor.setEnv2R2(
                                    static_cast<int>(rawValue)
                                );
                            }
                        );
                    }
                }
                else if (offset == KeygroupHeaderOffset::Env2::L2)
                {
                    DBG("ENV2 L2 RECEIVED = " + juce::String(rawValue));

                    if (keygroup >= 0 &&
                        keygroup < static_cast<int>(loadedProgram.keygroups.size()))
                    {
                        loadedProgram.keygroups[keygroup].env2.l2 =
                            static_cast<int>(rawValue);

                        juce::MessageManager::callAsync(
                            [this, rawValue]()
                            {
                                keyGroupEditor.setEnv2L2(
                                    static_cast<int>(rawValue)
                                );
                            }
                        );
                    }
                }
                else if (offset == KeygroupHeaderOffset::Env2::R3)
                {
                    DBG("ENV2 R3 RECEIVED = " + juce::String(rawValue));

                    if (keygroup >= 0 &&
                        keygroup < static_cast<int>(loadedProgram.keygroups.size()))
                    {
                        loadedProgram.keygroups[keygroup].env2.r3 =
                            static_cast<int>(rawValue);

                        juce::MessageManager::callAsync(
                            [this, rawValue]()
                            {
                                keyGroupEditor.setEnv2R3(
                                    static_cast<int>(rawValue)
                                );
                            }
                        );
                    }
                }
                else if (offset == KeygroupHeaderOffset::Env2::L3)
                {
                    DBG("ENV2 L3 RECEIVED = " + juce::String(rawValue));

                    if (keygroup >= 0 &&
                        keygroup < static_cast<int>(loadedProgram.keygroups.size()))
                    {
                        loadedProgram.keygroups[keygroup].env2.l3 =
                            static_cast<int>(rawValue);

                        juce::MessageManager::callAsync(
                            [this, rawValue]()
                            {
                                keyGroupEditor.setEnv2L3(
                                    static_cast<int>(rawValue)
                                );
                            }
                        );
                    }
                }
                else if (offset == KeygroupHeaderOffset::Env2::R4)
                {
                    DBG("ENV2 R4 RECEIVED = " + juce::String(rawValue));

                    if (keygroup >= 0 &&
                        keygroup < static_cast<int>(loadedProgram.keygroups.size()))
                    {
                        loadedProgram.keygroups[keygroup].env2.r4 =
                            static_cast<int>(rawValue);

                        juce::MessageManager::callAsync(
                            [this, rawValue]()
                            {
                                keyGroupEditor.setEnv2R4(
                                    static_cast<int>(rawValue)
                                );
                            }
                        );
                    }
                }
                else if (offset == KeygroupHeaderOffset::Env2::L4)
                {
                    DBG("ENV2 L4 RECEIVED = " + juce::String(rawValue));

                    if (keygroup >= 0 &&
                        keygroup < static_cast<int>(loadedProgram.keygroups.size()))
                    {
                        loadedProgram.keygroups[keygroup].env2.l4 =
                            static_cast<int>(rawValue);

                        juce::MessageManager::callAsync(
                            [this, rawValue]()
                            {
                                keyGroupEditor.setEnv2L4(
                                    static_cast<int>(rawValue)
                                );
                            }
                        );
                    }
                }
                

                if (offset == KeygroupHeaderOffset::Filter::FILFRQ
                    && keygroup >= 0
                    && keygroup < static_cast<int>(loadedProgram.keygroups.size()))
                {
                    loadedProgram.keygroups[keygroup].filter.freq =
                        static_cast<int>(rawValue);

                    DBG(
                        "UPDATE KG FILTER FREQ KG="
                        + juce::String(keygroup)
                        + " VALUE="
                        + juce::String((int)rawValue)
                    );

                    if (keygroup == currentKeygroup)
                    {
                        juce::MessageManager::callAsync(
                            [this, rawValue]()
                            {
                                keyGroupEditor.setFilterFreq(
                                    static_cast<int>(rawValue)
                                );
                            }
                        );
                    }
                }

                if (offset == KeygroupHeaderOffset::Mod::Filter1
                    && keygroup >= 0
                    && keygroup < static_cast<int>(loadedProgram.keygroups.size()))
                {
                    loadedProgram.keygroups[keygroup].modFilter1 =
                        signedValue;

                    if (keygroup == currentKeygroup)
                    {
                        juce::MessageManager::callAsync(
                            [this, signedValue]()
                            {
                                programEditor.setModFilter1Amount(
                                    signedValue
                                );
                            }
                        );
                    }

                    DBG(
                        "UPDATE KG MODFILTER1 KG="
                        + juce::String(keygroup)
                        + " VALUE="
                        + juce::String(signedValue)
                    );

                    // Filter1 reply received -> request Filter2 next
                    if (extraKgParamRequest ==
                        ExtraKgParamRequest::filter1
                        && programNumber == extraKgParamProgram
                        && keygroup == extraKgParamKeygroup)
                    {
                        DBG(
                            "EXTRA KG PARAM FILTER1 RECEIVED"
                            " -> NEXT FILTER2"
                        );

                        extraKgParamRequest =
                            ExtraKgParamRequest::filter2;

                        sendNextExtraKgParamRequest();
                    }

                }

                if (offset == KeygroupHeaderOffset::Mod::Filter2
                    && keygroup >= 0
                    && keygroup < static_cast<int>(loadedProgram.keygroups.size()))
                {
                    loadedProgram.keygroups[keygroup].modFilter2 =
                        signedValue;

                    if (keygroup == currentKeygroup)
                    {
                        juce::MessageManager::callAsync(
                            [this, signedValue]()
                            {
                                programEditor.setModFilter2Amount(
                                    signedValue
                                );
                            }
                        );
                    }

                    DBG(
                        "UPDATE KG MODFILTER2 KG="
                        + juce::String(keygroup)
                        + " VALUE="
                        + juce::String(signedValue)
                    );

                    // Filter2 reply received -> request Filter3 next
                    if (extraKgParamRequest ==
                        ExtraKgParamRequest::filter2
                        && programNumber == extraKgParamProgram
                        && keygroup == extraKgParamKeygroup)
                    {
                        DBG(
                            "EXTRA KG PARAM FILTER2 RECEIVED"
                            " -> NEXT FILTER3"
                        );

                        extraKgParamRequest =
                            ExtraKgParamRequest::filter3;

                        sendNextExtraKgParamRequest();
                    }

                }

                if (offset == KeygroupHeaderOffset::Mod::Filter3
                    && keygroup >= 0
                    && keygroup < static_cast<int>(
                        loadedProgram.keygroups.size()))
                {
                    loadedProgram.keygroups[keygroup].modFilter3 =
                        signedValue;

                    if (keygroup == currentKeygroup)
                    {
                        juce::MessageManager::callAsync(
                            [this, signedValue]()
                            {
                                programEditor.setModFilter3Amount(
                                    signedValue
                                );
                            }
                        );
                    }

                    DBG(
                        "UPDATE KG MODFILTER3 KG="
                        + juce::String(keygroup)
                        + " VALUE="
                        + juce::String(signedValue)
                    );

                    // ========================================
                    // Sequential extra KG request complete
                    // ========================================

                    if (extraKgParamRequest ==
                        ExtraKgParamRequest::filter3
                        && programNumber == extraKgParamProgram
                        && keygroup == extraKgParamKeygroup)
                    {
                        DBG(
                            "EXTRA KG PARAM FILTER3 RECEIVED"
                            " -> COMPLETE"
                        );

                        extraKgParamRequest =
                            ExtraKgParamRequest::none;

                        extraKgParamProgram = -1;
                        extraKgParamKeygroup = -1;

                        // このKGの追加パラメータ取得が完全に終わったので
                        // 次のKGへ進む
                        continueKeygroupLoading();
                    }
                }



                if (offset == KeygroupHeaderOffset::Mod::ModVPitch
                    && keygroup >= 0
                    && keygroup < static_cast<int>(loadedProgram.keygroups.size()))
                {
                    loadedProgram.keygroups[keygroup].modVPitch =
                        signedValue;

                    DBG(
                        "UPDATE KG MODVPITCH KG="
                        + juce::String(keygroup)
                        + " VALUE="
                        + juce::String(signedValue)
                    );

                    if (keygroup == currentKeygroup)
                    {
                        juce::MessageManager::callAsync(
                            [this, signedValue]()
                            {
                                programEditor.setModPitchAmount(
                                    signedValue
                                );
                            }
                        );
                    }
                }


                break;
            }

            if (size == 15)
            {
                const int programNumber =
                    (data[4] & 0x7F)
                    | ((data[5] & 0x7F) << 7);

                const int keygroup =
                    data[6] & 0x7F;

                const int offset =
                    (data[7] & 0x7F)
                    | ((data[8] & 0x7F) << 7);

                const int length =
                    (data[9] & 0x7F)
                    | ((data[10] & 0x7F) << 7);

                const uint8_t byte0 =
                    (data[11] & 0x0F)
                    | ((data[12] & 0x0F) << 4);

                const uint8_t byte1 =
                    (data[13] & 0x0F)
                    | ((data[14] & 0x0F) << 4);

                const int value =
                    static_cast<int>(byte0)
                    | (static_cast<int>(byte1) << 8);

                DBG(
                    "SHORT 0x2A SIZE15"
                    + juce::String(" PROGRAM=") + juce::String(programNumber)
                    + " KG=" + juce::String(keygroup)
                    + " OFFSET=" + juce::String(offset)
                    + " LENGTH=" + juce::String(length)
                    + " VALUE=" + juce::String(value)
                );

                if (offset == KeygroupHeaderOffset::Filter::FILQ
                    && keygroup >= 0
                    && keygroup < static_cast<int>(loadedProgram.keygroups.size()))
                {
                    loadedProgram.keygroups[keygroup].filter.resonance = value;

                    DBG(
                        "UPDATE KG RESONANCE KG="
                        + juce::String(keygroup)
                        + " VALUE="
                        + juce::String(value)
                    );

                    if (keygroup == currentKeygroup)
                    {
                        juce::MessageManager::callAsync(
                            [this, value]()
                            {
                                keyGroupEditor.setResonance(value);
                            }
                        );
                    }

                    break;
                }
            }


            handleKeygroupHeaderResponse();
            break;
        }



        case 0x2B:
        {
            handleKeygroupFullResponse();
            break;
        }

        case 0x07:
        {
            handleRPDataResponse();
            break;
        }

        case 0x2C:
        {
            handleSampleHeaderResponse();
            break;



        }


        case 0x03:
        {


            handleProgramListResponse(data, size);
            break;
        }

        case 0x09:
        {
            handleKeygroupDataResponse(message);
            break;
        }








        default:
            DBG("Unknown Opcode = 0x" + juce::String::toHexString((int)opcode));
            return;


        }
    captureIndex++;
    }

void MainComponent::handleSampleHeaderResponse()
{


    auto* p =
        (uint8_t*)programBuffer.getData();

    const int rawSize =
        (int)programBuffer.getSize();

    // ==============================
    // Raw SysEx�擪�m�F
    // ==============================

    for (int i = 0;
        i < juce::jmin(30, rawSize);
        ++i)
    {
        DBG(
            juce::String(i)
            + " : "
            + juce::String::toHexString(p[i])
        );
    }

    DBG("=== RAW SYSEX END ===");

    // ==============================
    // Raw SysEx�����m�F
    // ==============================

    for (int i =
        juce::jmax(0, rawSize - 30);
        i < rawSize;
        ++i)
    {
        DBG(
            juce::String(i)
            + " : 0x"
            + juce::String::toHexString(p[i])
        );
    }

    // ==============================
    // Decode
    // ==============================

//            decoded =
//                decodeSampleHeader(programBuffer);

    auto decoded =
        decodeSampleHeader(programBuffer);



    if (decoded.size() < 141)
    {
        DBG("INVALID SAMPLE HEADER SIZE");
        return;
    }

    // ========================================
// SAMPLE RENAME READBACK
// ========================================

    if (pendingSampleRenameReadbackId >= 0)
    {
        const int sampleId =
            pendingSampleRenameReadbackId;

        pendingSampleRenameReadbackId = -1;

        SampleHeader sh =
            SampleHeaderParser::parse(
                decoded,
                sampleId
            );

        sh.id = sampleId;

        DBG(
            "SAMPLE RENAME READBACK ID="
            + juce::String(sampleId)
            + " NAME=["
            + sh.name
            + "]"
        );

        // Hardware readback is now authoritative
        sampleHeaders[sampleId] = sh;

        residentSamples[sampleId] =
            sh.name.trim();

        for (auto& kg : loadedProgram.keygroups)
        {
            for (auto& zone : kg.zones)
            {
                if (zone.sampleId == sampleId)
                {
                    zone.sampleName =
                        sh.name;
                }
            }
        }


        juce::MessageManager::callAsync(
            [this, sh, sampleId]()
            {
                sampleHeaderEditor.setSampleHeader(
                    sh
                );

                velocityZoneEditor.setResidentSamples(
                    residentSamples
                );

                programTree.setProgram(
                    loadedProgram,
                    sampleHeaders
                );

                DBG(
                    "SAMPLE RENAME READBACK UI UPDATED ID="
                    + juce::String(sampleId)
                );
            }
        );

        return;
    }

    // ==============================
    // Loop area debug
    // ==============================

    DBG("=== LOOP HEADER ===");

    for (int i = 16;
        i < juce::jmin(86, (int)decoded.size());
        ++i)
    {
        DBG(
            juce::String::formatted(
                "%03d : 0x%02X (%d)",
                i,
                (unsigned int)decoded[i],
                (unsigned int)decoded[i]
            )
        );
    }

    // ==============================
    // Decode�擪�m�F
    // ==============================

    for (int i = 0;
        i < juce::jmin(20, (int)decoded.size());
        ++i)
    {
        DBG(
            "decoded "
            + juce::String(i)
            + " : "
            + juce::String::toHexString(decoded[i])
        );
    }

    // ==============================
    // pending request�m�F
    // ==============================

    if (pendingSampleRequests.empty())
    {
        DBG(
            "SAMPLE HEADER RECEIVED "
            "BUT NO PENDING SAMPLE REQUEST"
        );

        return;
    }

    // ==============================
    // �Ώ�Sample ID���擾
    // ==============================


    if (activeSampleHeaderRequestId < 0)
    {
        DBG(
            "SAMPLE HEADER RECEIVED "
            "BUT NO ACTIVE SAMPLE REQUEST"
        );

        return;
    }

    const int sampleId =
        activeSampleHeaderRequestId;

    SampleHeader sh =
        SampleHeaderParser::parse(
            decoded,
            sampleId
        );

    sh.id = sampleId;

\

    DBG(
        "SAMPLE HEADER RECEIVED ID="
        + juce::String(sampleId)
        + " NAME=["
        + sh.name.trim()
        + "]"
    );




    // ==============================
    // Store
    // ==============================

    sampleHeaders[sh.id] = sh;




    if (currentKeygroup >= 0 &&
        currentKeygroup < (int)loadedProgram.keygroups.size())
    {
        const auto& kg =
            loadedProgram.keygroups[currentKeygroup];

        if (currentZone >= 0 &&
            currentZone < (int)kg.zones.size())
        {
            const auto& selectedZone =
                kg.zones[currentZone];

            if (selectedZone.sampleId == sh.id)
            {
                DBG(
                    "UPDATE SAMPLE HEADER EDITOR ID="
                    + juce::String(sh.id)
                );

                juce::MessageManager::callAsync(
                    [this, sh]
                    {
                        sampleHeaderEditor.setSampleHeader(
                            sh
                        );
                    }
                );
            }
        }
    }



    for (const auto& [id, header] :
        sampleHeaders)
    {
        DBG(
            "HEADER ID="
            + juce::String(id)
            + " NAME=["
            + header.name
            + "]"
        );
    }

    pendingSampleRequests.erase(
        sampleId
    );

    activeSampleHeaderRequestId = -1;

    trySendNextSampleHeader();

    if (pendingSampleRequests.empty())
    {
        DBG("ALL SAMPLE HEADERS RECEIVED");

        juce::MessageManager::callAsync(
            [this]
            {
                DBG("CALL ASYNC START");

                // Sample Name -> Sample ID �ĉ���
                resolveAllSampleIds();

                DBG(
                    "AFTER resolveAllSampleIds"
                );

                // �m�F���O
                for (int k = 0;
                    k <
                    (int)loadedProgram
                    .keygroups.size();
                    ++k)
                {
                    const auto& kg =
                        loadedProgram.keygroups[k];

                    for (int z = 0;
                        z <
                        (int)kg.zones.size();
                        ++z)
                    {
                        const auto& zone =
                            kg.zones[z];

                        DBG(
                            "KG="
                            + juce::String(k)
                            + " ZONE="
                            + juce::String(z)
                            + " SAMPLE=["
                            + zone.sampleName
                            + "] ID="
                            + juce::String(
                                zone.sampleId
                            )
                        );
                    }
                }

                // �ŐV�f�[�^��Tree�č\�z
                programTree.setProgram(
                    loadedProgram,
                    sampleHeaders
                );


            }
        );
    }
    else
    {
        DBG(
            "WAITING FOR MORE SAMPLE HEADERS. "
            "pending="
            + juce::String(
                (int)pendingSampleRequests.size()
            )
        );
        for (int id : pendingSampleRequests)
        {
            DBG(
                "  PENDING SAMPLE ID="
                + juce::String(id)
            );
        }
    }

    // ==============================
    // Full decoded dump
    // ==============================

    for (int i = 0;
        i < (int)decoded.size();
        ++i)
    {
        DBG(
            juce::String(i)
            + " : 0x"
            + juce::String::toHexString(
                decoded[i]
            )
        );
    }

    for (int i = 1; i < 4; ++i)
    {
        DBG(
            "LOOP "
            + juce::String(i + 1)
            + " POSITION = "
            + juce::String(sh.loops[i].position)
        );

        DBG(
            "LOOP "
            + juce::String(i + 1)
            + " LENGTH = "
            + juce::String(sh.loops[i].length, 3)
        );

        DBG(
            "LOOP "
            + juce::String(i + 1)
            + " DWELL = "
            + juce::String(sh.loops[i].dwell)
        );
    }


    saveDecodedDump(
        "sample_header_0.bin",
        decoded
    );






}

void MainComponent::handleSampleListResponse()
{


    parseSLIST(programBuffer);



    for (int i = 0; i < programBuffer.getSize(); ++i)
    {
        DBG(
            juce::String(i)
            + " : 0x"
            + juce::String::toHexString(programBuffer[i])
        );
    }


//    break;
}

void MainComponent::handleProgramListResponse(
    const uint8_t* data,
    int size)
{

    if (pendingMidiDeviceSave)
    {
        DBG("S3000XL PLIST RESPONSE RECEIVED");
        DBG("CONFIRMED S3000XL MIDI CONNECTION");

        saveS3000MidiDevices(
            pendingMidiInputDevice,
            pendingMidiOutputDevice
        );

        pendingMidiDeviceSave = false;
    }

    auto rawCopy =
        std::vector<uint8_t>(
            data,
            data + size
        );

    juce::MessageManager::callAsync(
        [this, rawCopy]()
        {
            parsePLIST(rawCopy);
        }
    );
}

void MainComponent::handleKeygroupDataResponse(
    const juce::MidiMessage& message)
{



    auto* raw = programBuffer.getData();
    auto size = programBuffer.getSize();

    for (int i = 0; i < size; i++)
    {
        DBG(
            juce::String(i)
            + " : 0x"
            + juce::String::toHexString(
                ((uint8_t*)raw)[i]
            )
        );
    }

    auto decoded =
        decodeKData(
            message.getSysExData(),
            message.getSysExDataSize()
        );

    DBG(
        "KDATA DECODED SIZE="
        + juce::String((int)decoded.size())
    );

    if (decoded.size() > 154)
    {
        DBG(
            "KDATA OFFSET154 RAW="
            + juce::String((int)decoded[154])
            + " VALUE="
            + juce::String(
                (int)static_cast<int8_t>(decoded[154])
            )
        );
    }

    DBG("=== KDATA AFTER HARDWARE KNOB ===");

    for (int i = 0; i < (int)decoded.size(); ++i)
    {
        DBG(
            juce::String(i)
            + " = 0x"
            + juce::String::toHexString((int)decoded[i])
        );
    }



    saveDecodedDump(
        "keygroup_basic",
        decoded
    );






    if (decoded.size() < 132)
    {
        DBG("Invalid KDATA SIZE");
        return;
    }


    // ========================================
    // Parse Keygroup
    // ========================================

    Keygroup kg =
        KeygroupParser::parse(
            decoded,
            residentSamples
        );


    //DBG("=== KG RAW DATA ===");

    //for (int i = 0; i < (int)kg.rawData.size(); ++i)
    //{
    //    DBG(
    //        juce::String(i)
    //        + " = "
    //        + juce::String((int)kg.rawData[i])
    //    );
    //}



    // ========================================
    // Resolve Sample ID
    // ========================================

    for (auto& zone : kg.zones)
    {
        const auto sampleName =
            zone.sampleName.trim();

        if (sampleName.isEmpty())
        {
            zone.sampleId = -1;
            continue;
        }

        DBG(
            "RESOLVE SAMPLE NAME=["
            + sampleName
            + "]"
        );

        const int sampleIndex =
            findSampleId(sampleName);

        DBG(
            "findSampleId RESULT = "
            + juce::String(sampleIndex)
        );

        if (sampleIndex >= 0)
        {
            zone.sampleId = sampleIndex;

            DBG(
                "ASSIGNED SAMPLE ID = "
                + juce::String(zone.sampleId)
            );
        }
        else
        {
            zone.sampleId = -1;

            DBG(
                "FAILED SAMPLE RESOLVE = ["
                + sampleName
                + "]"
            );
        }
    }


    // ========================================
    // Store completed Keygroup
    // ========================================

    if (loadingKeygroup < 0)
    {
        DBG("INVALID loadingKeygroup");
        return;
    }

    if (loadingKeygroup >=
        (int)loadedProgram.keygroups.size())
    {
        loadedProgram.keygroups.resize(
            loadingKeygroup + 1
        );
    }

    loadedProgram.keygroups[loadingKeygroup] = kg;

    if (loadingKeygroup == currentKeygroup)
    {
        const int modVPitch =
            loadedProgram.keygroups[loadingKeygroup].modVPitch;

        DBG(
            "INITIAL KG MODVPITCH UI="
            + juce::String(modVPitch)
        );

        juce::MessageManager::callAsync(
            [this, modVPitch]()
            {
                programEditor.setModPitchAmount(
                    modVPitch
                );
            }
        );
    }

    const int requestProgram =
        loadedProgram.programNumber;

    const int requestKeygroup =
        loadingKeygroup;

    startExtraKgParamRequests(
        requestProgram,
        requestKeygroup
    );




    // ========================================
  // ���񃍁[�h����Map / Tree�𒀎��X�V
  // ========================================

    //const int mapUpdateIndex =
    //    loadingKeygroup;

    //const Program programForMap =
    //    loadedProgram;



    //juce::MessageManager::callAsync(
    //    [this, programForMap, mapUpdateIndex]()
    //    {


    //        // KeygroupMap
    //        keygroupMap.setProgram(
    //            programForMap
    //        );

    //        // ProgramTree
    //        programTree.setProgram(
    //            programForMap,
    //            sampleHeaders
    //        );


    //    }
    //);


    // ========================================
    // Request Sample Headers
    // ========================================

    //for (const auto& zone :
    //    loadedProgram.keygroups[loadingKeygroup].zones)
    //{
    //    if (zone.sampleId < 0)
    //        continue;

    //    // ���łɎ����Ă���Sample Header�Ȃ�
    //    // �ă��N�G�X�g���Ȃ�
    //    if (sampleHeaders.find(zone.sampleId)
    //        != sampleHeaders.end())
    //    {
    //        DBG(
    //            "SAMPLE HEADER ALREADY EXISTS ID="
    //            + juce::String(zone.sampleId)
    //        );

    //        continue;
    //    }





    //    sendSampleHeader(
    //        zone.sampleId
    //    );
    //}





    // ========================================
    // Loaded Program Debug
    // ========================================

    DBG("=== LOADED PROGRAM ===");

    for (int k = 0;
        k < loadedProgram.keygroups.size();
        ++k)
    {
        const auto& storedKg =
            loadedProgram.keygroups[k];



        for (int z = 0;
            z < storedKg.zones.size();
            ++z)
        {
            const auto& zone =
                storedKg.zones[z];


        }
    }



    //if (!pendingSampleRequests.empty())
    //{
    //    DBG(
    //        "WAITING FOR SAMPLE HEADERS. pending="
    //        + juce::String(
    //            (int)pendingSampleRequests.size()
    //        )
    //    );
    //}

    


}

void MainComponent::continueKeygroupLoading()
{
    // ========================================
    // Continue loading Keygroups
    // ========================================

    ++loadingKeygroup;

    if (loadingKeygroup < totalKeygroups)
    {
        const int nextProgram =
            loadedProgram.programNumber;

        const int nextKeygroup =
            loadingKeygroup;

        DBG(
            "QUEUE NEXT KG HEADER INDEX="
            + juce::String(nextKeygroup)
        );

        juce::MessageManager::callAsync(
            [this, nextProgram, nextKeygroup]()
            {
                DBG(
                    "SEND NEXT KG HEADER INDEX="
                    + juce::String(nextKeygroup)
                );

                sysExSender.sendKGHeader(
                    nextProgram,
                    nextKeygroup
                );
            }
        );

        return;
    }

    // ========================================
    // All Keygroups loaded
    // ========================================

    static int finishCount = 0;
    ++finishCount;

    // ========================================
// Build Sample Header request queue
// ========================================

    pendingSampleRequests.clear();
    pendingSampleRequestOrder.clear();
    activeSampleHeaderRequestId = -1;

    for (const auto& kg : loadedProgram.keygroups)
    {
        for (const auto& zone : kg.zones)
        {
            if (zone.sampleId < 0)
                continue;

            // Already loaded
            if (sampleHeaders.find(zone.sampleId)
                != sampleHeaders.end())
            {
                continue;
            }

            // insert() が true の時だけ新規ID
            const auto [it, inserted] =
                pendingSampleRequests.insert(zone.sampleId);

            if (inserted)
            {
                pendingSampleRequestOrder.push_back(
                    zone.sampleId
                );
            }
        }
    }

    DBG(
        "SAMPLE HEADER QUEUE BUILT"
        " unique="
        + juce::String(
            (int)pendingSampleRequests.size()
        )
        + " order="
        + juce::String(
            (int)pendingSampleRequestOrder.size()
        )
    );


    DBG(
        "=== KG LOAD FINISHED ==="
        " finishCount="
        + juce::String(finishCount)
        + " groups="
        + juce::String(loadedProgram.groups)
        + " keygroups.size="
        + juce::String(
            (int)loadedProgram.keygroups.size()
        )
        + " totalKeygroups="
        + juce::String(totalKeygroups)
    );


    const int restoreKeygroup =
        juce::jlimit(
            0,
            juce::jmax(0, totalKeygroups - 1),
            currentKeygroup
        );

    DBG(
        "ALL KG EXTRA PARAMS COMPLETE"
        " - RESTORE HARDWARE KG DISPLAY INDEX="
        + juce::String(restoreKeygroup)
    );

    restoringKeygroupDisplay = true;

    const int restoreProgram =
        loadedProgram.programNumber;

    juce::MessageManager::callAsync(
        [this, restoreProgram, restoreKeygroup]()
        {
            DBG(
                "SEND RESTORE KG HEADER INDEX="
                + juce::String(restoreKeygroup)
            );

            sysExSender.sendKGHeader(
                restoreProgram,
                restoreKeygroup
            );
        }
    );

    // 通信状態をリセット
    loadingKeygroup = 0;

    juce::MessageManager::callAsync(
        [this]
        {
            DBG(
                "=== FINAL MAP/TREE UPDATE ==="
                " groups="
                + juce::String(loadedProgram.groups)
                + " keygroups.size="
                + juce::String(
                    (int)loadedProgram.keygroups.size()
                )
            );

            constexpr int rowHeight = 32;

            const int contentHeight =
                juce::jmax(
                    200,
                    static_cast<int>(
                        loadedProgram.keygroups.size()
                        ) * rowHeight
                    + KeygroupMap::buttonAreaHeight
                    + 20
                );

            keygroupMap.setSize(
                juce::jmax(
                    1,
                    keygroupMapViewport.getWidth() - 16
                ),
                contentHeight
            );

            DBG("CALL keygroupMap.setProgram");

            keygroupMap.setProgram(
                loadedProgram
            );

            DBG("CALL programTree.setProgram");

            programTree.setProgram(
                loadedProgram,
                sampleHeaders
            );

            DBG("=== FINAL MAP/TREE UPDATE DONE ===");
        }
    );
}



void MainComponent::startExtraKgParamRequests(
    int programIndex,
    int keygroupIndex)
{
    extraKgParamProgram = programIndex;
    extraKgParamKeygroup = keygroupIndex;

    extraKgParamRequest =
        ExtraKgParamRequest::lfo1Pitch;

    sendNextExtraKgParamRequest();
}


void MainComponent::sendNextExtraKgParamRequest()
{
    if (extraKgParamRequest ==
        ExtraKgParamRequest::none)
    {
        return;
    }

    const int programIndex =
        extraKgParamProgram;

    const int keygroupIndex =
        extraKgParamKeygroup;

    const auto request =
        extraKgParamRequest;

    juce::MessageManager::callAsync(
        [this,
        programIndex,
        keygroupIndex,
        request]()
        {
            int offset = -1;

            switch (request)
            {
            case ExtraKgParamRequest::lfo1Pitch:
                offset =
                    KeygroupHeaderOffset::Mod::Lfo1Pitch;
                break;

            case ExtraKgParamRequest::filter1:
                offset =
                    KeygroupHeaderOffset::Mod::Filter1;
                break;

            case ExtraKgParamRequest::filter2:
                offset =
                    KeygroupHeaderOffset::Mod::Filter2;
                break;

            case ExtraKgParamRequest::filter3:
                offset =
                    KeygroupHeaderOffset::Mod::Filter3;
                break;

            default:
                return;
            }

            DBG(
                "SEND EXTRA KG PARAM"
                " KG="
                + juce::String(keygroupIndex)
                + " OFFSET="
                + juce::String(offset)
            );

            sysExSender.sendKeygroupHeaderByteRequest(
                programIndex,
                keygroupIndex,
                offset
            );
        }
    );
}


void MainComponent::handleCommandReply(
    const uint8_t* data,
    int size)
{
    DBG("COMMAND REPLY RECEIVED");

    if (size < 5)
        return;

    uint8_t result =
        data[4];

    DBG(
        "AKAI REPLY CODE = "
        + juce::String((int)result)
    );


    // ========================================
// SAMPLE RENAME Reply
// ========================================

    if (waitingForSampleRenameReply)
    {
        waitingForSampleRenameReply = false;

        const int sampleId =
            pendingRenamedSampleId;

        pendingRenamedSampleId = -1;

        if (result != 0)
        {
            DBG(
                "SAMPLE RENAME FAILED CODE="
                + juce::String((int)result)
            );

            return;
        }

        DBG(
            "SAMPLE RENAME REPLY OK ID="
            + juce::String(sampleId)
        );

        pendingSampleRenameReadbackId =
            sampleId;

        // Refresh resident sample names
        sysExSender.sendRSLIST();

        // Read actual Sample Header back from hardware
        sysExSender.sendSampleHeader(
            sampleId
        );

        return;
    }

    // ========================================
// ADD PROGRAM - STEP 1 PDATA Reply
// ========================================

    if (pendingAddProgramStage ==
        PendingAddProgramStage::
        waitingForProgramReply)
    {
        if (result != 0)
        {
            DBG("ADD PROGRAM: PDATA FAILED");

            pendingAddProgramStage =
                PendingAddProgramStage::none;

            pendingNewProgramNumber = -1;

            return;
        }



        auto encodedKeygroup =
            KeygroupEncoder::encode(
                pendingNewProgramKeygroup
            );

        if (encodedKeygroup.empty())
        {


            pendingAddProgramStage =
                PendingAddProgramStage::none;

            pendingNewProgramNumber = -1;

            return;
        }

        pendingAddProgramStage =
            PendingAddProgramStage::
            waitingForKeygroupReply;



        sysExSender.sendKeygroupData(
            pendingNewProgramNumber,
            0,
            encodedKeygroup
        );

        return;
    }


    // ========================================
    // ADD PROGRAM - STEP 2 KDATA Reply
    // ========================================

    if (pendingAddProgramStage ==
        PendingAddProgramStage::
        waitingForKeygroupReply)
    {
        if (result != 0)
        {
            DBG("ADD PROGRAM: KDATA FAILED");

            pendingAddProgramStage =
                PendingAddProgramStage::none;

            pendingNewProgramNumber = -1;

            return;
        }

        DBG("ADD PROGRAM: KDATA REPLY OK");

        pendingProgramSelection =
            pendingNewProgramNumber;

        pendingNewProgramNumber = -1;

        pendingAddProgramStage =
            PendingAddProgramStage::none;

        // �ŐV��Program�ꗗ�����@�����蒼��
        sysExSender.sendRPLIST();

        return;
    }


    // ========================================
// ADD KG - STEP 1 KDATA Reply
// ========================================

    if (pendingAddStage ==
        PendingAddStage::
        waitingForKeygroupReply)
    {
        if (result != 0)
        {
            DBG("ADD KG: KDATA FAILED");

            pendingAddStage =
                PendingAddStage::none;

            pendingAddKeygroupIndex = -1;

            return;
        }

        DBG(
            "ADD KG: KDATA REPLY OK"
        );

        // ========================================
        // GROUPS + 1 ��PDATA�����
        // ========================================

        auto newProgram =
            loadedProgram;

        newProgram.groups =
            loadedProgram.groups + 1;

        auto encodedProgram =
            ProgramEncoder::encode(
                newProgram
            );

        if (encodedProgram.empty())
        {
            DBG(
                "ADD KG: PROGRAM ENCODE FAILED"
            );

            pendingAddStage =
                PendingAddStage::none;

            pendingAddKeygroupIndex = -1;

            return;
        }

        pendingAddStage =
            PendingAddStage::
            waitingForProgramReply;

        DBG(
            "STEP 2: SEND PDATA GROUPS + 1"
        );
        DBG("PDATA CALLER 6511");
        sysExSender.sendProgramData(
            loadedProgram.programNumber,
            encodedProgram
        );

        return;
    }


    // ========================================
    // ADD KG - STEP 2 PDATA Reply
    // ========================================

    if (pendingAddStage ==
        PendingAddStage::
        waitingForProgramReply)
    {
        if (result != 0)
        {
            DBG("ADD KG: PDATA FAILED");

            pendingAddStage =
                PendingAddStage::none;

            pendingAddKeygroupIndex = -1;

            return;
        }

        DBG(
            "ADD KG: PDATA REPLY OK"
        );

        pendingAddStage =
            PendingAddStage::none;

        const int addedIndex =
            pendingAddKeygroupIndex;

        pendingAddKeygroupIndex = -1;

        juce::MessageManager::callAsync(
            [this]
            {
                constexpr int rowHeight = 32;

                const int contentHeight =
                    juce::jmax(
                        200,
                        static_cast<int>(
                            loadedProgram.keygroups.size()
                            ) * rowHeight
                        + KeygroupMap::buttonAreaHeight
                        + 20
                    );

                keygroupMap.setSize(
                    juce::jmax(
                        1,
                        keygroupMapViewport.getWidth() - 16
                    ),
                    contentHeight
                );

                keygroupMap.setProgram(
                    loadedProgram
                );

                programTree.setProgram(
                    loadedProgram,
                    sampleHeaders
                );

                // 初回ロード完了
                initialProgramLoad = false;

                // 起動時はProgramタブ
                editorTabs.setCurrentTabIndex(0);
            }
        );

        return;
    }


    // ========================================
    // Delete Keygroup reply
    // ========================================

    if (waitingForDeleteReply)
    {
        waitingForDeleteReply = false;

        if (result == 0)
        {
            DBG("DELETE REPLY OK");

            const int deletedIndex =
                pendingDeleteKeygroup;

            pendingDeleteKeygroup = -1;

            juce::MessageManager::callAsync(
                [this, deletedIndex]()
                {
                    if (deletedIndex < 0 ||
                        deletedIndex >=
                        static_cast<int>(
                            loadedProgram.keygroups.size()
                            ))
                    {
                        DBG("DELETE LOCAL UPDATE: INVALID INDEX");
                        return;
                    }

                    // =========================
                    // Model�X�V
                    // =========================

                    loadedProgram.keygroups.erase(
                        loadedProgram.keygroups.begin()
                        + deletedIndex
                    );

                    loadedProgram.groups =
                        static_cast<int>(
                            loadedProgram.keygroups.size()
                            );

                    totalKeygroups =
                        loadedProgram.groups;

                    constexpr int rowHeightForMap = 32;
                    constexpr int extraHeight = 80;

                    const int contentHeight =
                        juce::jmax(
                            200,
                            loadedProgram.groups
                            * rowHeightForMap
                            + extraHeight
                        );

                    keygroupMap.setSize(
                        juce::jmax(
                            1,
                            keygroupMapViewport.getWidth() - 16
                        ),
                        contentHeight
                    );




                    // =========================
                    // �I���ʒu��␳
                    // =========================

                    if (currentKeygroup >=
                        loadedProgram.groups)
                    {
                        currentKeygroup =
                            loadedProgram.groups - 1;
                    }

                    if (currentKeygroup < 0)
                        currentKeygroup = 0;

                    currentZone = 0;

                    // =========================
                    // UI�X�V
                    // =========================

                    keygroupMap.setProgram(
                        loadedProgram
                    );

                    programTree.setProgram(
                        loadedProgram,
                        sampleHeaders
                    );

                    if (!loadedProgram.keygroups.empty())
                    {
                        auto& kg =
                            loadedProgram.keygroups[
                                currentKeygroup
                            ];

                        keyGroupEditor.setKeygroup(
                            kg,
                            currentKeygroup
                        );

                        if (!kg.zones.empty())
                        {
                            velocityZoneEditor.setZone(
                                kg.zones[0]
                            );
                        }
                    }

                    DBG(
                        "DELETE LOCAL UI UPDATE COMPLETE"
                    );
                }
            );
        }
        else
        {
            DBG("DELETE KEYGROUP FAILED");
            pendingDeleteKeygroup = -1;
        }

        return;
    }

    if (waitingForFilterReply)
    {
        waitingForFilterReply = false;

        DBG("FILTER REPLY time="
            + juce::String(juce::Time::getMillisecondCounter()));

        if (pendingFilterFreqValue >= 0 &&
            pendingFilterFreqKeygroup >= 0)
        {
            const int value =
                pendingFilterFreqValue;

            const int keygroupIndex =
                pendingFilterFreqKeygroup;

            pendingFilterFreqValue = -1;
            pendingFilterFreqKeygroup = -1;

            waitingForFilterReply = true;

            const int programIndex =
                currentProgramIndex;

            DBG(
                "FILTER FREQ QUEUE PENDING VALUE="
                + juce::String(value)
            );

            juce::MessageManager::callAsync(
                [this, programIndex, keygroupIndex, value]()
                {
                    DBG(
                        "FILTER FREQ SEND PENDING VALUE="
                        + juce::String(value)
                    );

                    sysExSender.sendKeygroupByte(
                        programIndex,
                        keygroupIndex,
                        KeygroupHeaderOffset::Filter::FILFRQ,
                        static_cast<uint8_t>(value)
                    );
                }
            );

            return;
        }

        if (pendingResonanceValue >= 0 &&
            pendingResonanceKeygroup >= 0)
        {
            const int value =
                pendingResonanceValue;

            const int keygroupIndex =
                pendingResonanceKeygroup;

            pendingResonanceValue = -1;
            pendingResonanceKeygroup = -1;

            waitingForFilterReply = true;

            const int programIndex =
                currentProgramIndex;

            DBG(
                "FILTER RESONANCE QUEUE PENDING VALUE="
                + juce::String(value)
            );

            juce::MessageManager::callAsync(
                [this, programIndex, keygroupIndex, value]()
                {
                    DBG(
                        "FILTER RESONANCE SEND PENDING VALUE="
                        + juce::String(value)
                    );

                    sysExSender.sendKeygroupWord(
                        programIndex,
                        keygroupIndex,
                        KeygroupHeaderOffset::Filter::FILQ,
                        static_cast<uint16_t>(value)
                    );
                }
            );

            return;
        }

        if (pendingEnv2R1Value >= 0 &&
            pendingEnv2R1Keygroup >= 0)
        {
            const int value =
                pendingEnv2R1Value;

            const int keygroupIndex =
                pendingEnv2R1Keygroup;

            pendingEnv2R1Value = -1;
            pendingEnv2R1Keygroup = -1;

            waitingForFilterReply = true;

            const int programIndex =
                loadedProgram.programNumber;

            juce::MessageManager::callAsync(
                [this, programIndex, keygroupIndex, value]()
                {
                    sysExSender.sendKeygroupHeaderByte(
                        programIndex,
                        keygroupIndex,
                        static_cast<int>(
                            KeygroupHeaderOffset::Env2::R1
                            ),
                        value
                    );
                }
            );

            return;
        }

        if (pendingEnv2L1Value >= 0 &&
            pendingEnv2L1Keygroup >= 0)
        {
            const int value =
                pendingEnv2L1Value;

            const int keygroupIndex =
                pendingEnv2L1Keygroup;

            pendingEnv2L1Value = -1;
            pendingEnv2L1Keygroup = -1;

            waitingForFilterReply = true;

            const int programIndex =
                loadedProgram.programNumber;

            juce::MessageManager::callAsync(
                [this, programIndex, keygroupIndex, value]()
                {
                    sysExSender.sendKeygroupHeaderByte(
                        programIndex,
                        keygroupIndex,
                        static_cast<int>(
                            KeygroupHeaderOffset::Env2::L1
                            ),
                        value
                    );
                }
            );

            return;
        }

        if (pendingModFilter1Value != -1000 &&
            pendingModFilter1Keygroup >= 0)
        {
            const int value =
                pendingModFilter1Value;

            const int keygroupIndex =
                pendingModFilter1Keygroup;

            pendingModFilter1Value = -1000;
            pendingModFilter1Keygroup = -1;

            waitingForFilterReply = true;

            // MIDI callbackを抜けた後でも
            // 正しいProgram番号を使えるようにコピーしておく
            const int programIndex =
                loadedProgram.programNumber;

            DBG(
                "MOD FILTER1 QUEUE PENDING"
                " KG="
                + juce::String(keygroupIndex)
                + " VALUE="
                + juce::String(value)
            );

            juce::MessageManager::callAsync(
                [this, programIndex, keygroupIndex, value]()
                {
                    DBG(
                        "MOD FILTER1 SEND PENDING"
                        " KG="
                        + juce::String(keygroupIndex)
                        + " VALUE="
                        + juce::String(value)
                    );

                    sysExSender.sendKeygroupHeaderByte(
                        programIndex,
                        keygroupIndex,
                        KeygroupHeaderOffset::Mod::Filter1,
                        value
                    );
                }
            );

            return;
        }

        if (pendingModFilter2Value != -1000 &&
            pendingModFilter2Keygroup >= 0)
        {
            const int value =
                pendingModFilter2Value;

            const int keygroupIndex =
                pendingModFilter2Keygroup;

            pendingModFilter2Value = -1000;
            pendingModFilter2Keygroup = -1;

            waitingForFilterReply = true;

            DBG(
                "MOD FILTER2 SEND PENDING"
                " KG="
                + juce::String(keygroupIndex)
                + " VALUE="
                + juce::String(value)
            );

            sysExSender.sendKeygroupHeaderByte(
                loadedProgram.programNumber,
                keygroupIndex,
                KeygroupHeaderOffset::Mod::Filter2,
                value
            );

            return;
        }

        if (pendingModFilter3Value != -1000 &&
            pendingModFilter3Keygroup >= 0)
        {
            const int value =
                pendingModFilter3Value;

            const int keygroupIndex =
                pendingModFilter3Keygroup;

            pendingModFilter3Value = -1000;
            pendingModFilter3Keygroup = -1;

            waitingForFilterReply = true;

            DBG(
                "MOD FILTER3 SEND PENDING"
                " KG="
                + juce::String(keygroupIndex)
                + " VALUE="
                + juce::String(value)
            );

            sysExSender.sendKeygroupHeaderByte(
                loadedProgram.programNumber,
                keygroupIndex,
                KeygroupHeaderOffset::Mod::Filter3,
                value
            );

            return;
        }


        if (pendingEnv2R2Value >= 0 &&
            pendingEnv2R2Keygroup >= 0)
        {
            const int value = pendingEnv2R2Value;
            const int keygroupIndex = pendingEnv2R2Keygroup;

            pendingEnv2R2Value = -1;
            pendingEnv2R2Keygroup = -1;

            waitingForFilterReply = true;

            const int programIndex =
                loadedProgram.programNumber;

            juce::MessageManager::callAsync(
                [this, programIndex, keygroupIndex, value]()
                {
                    sysExSender.sendKeygroupHeaderByte(
                        programIndex,
                        keygroupIndex,
                        static_cast<int>(
                            KeygroupHeaderOffset::Env2::R2
                            ),
                        value
                    );
                }
            );

            return;
        }


        if (pendingEnv2L2Value >= 0 &&
            pendingEnv2L2Keygroup >= 0)
        {
            const int value = pendingEnv2L2Value;
            const int keygroupIndex = pendingEnv2L2Keygroup;

            pendingEnv2L2Value = -1;
            pendingEnv2L2Keygroup = -1;

            waitingForFilterReply = true;

            const int programIndex =
                loadedProgram.programNumber;

            juce::MessageManager::callAsync(
                [this, programIndex, keygroupIndex, value]()
                {
                    sysExSender.sendKeygroupHeaderByte(
                        programIndex,
                        keygroupIndex,
                        static_cast<int>(
                            KeygroupHeaderOffset::Env2::L2
                            ),
                        value
                    );
                }
            );

            return;
        }


        if (pendingEnv2R3Value >= 0 &&
            pendingEnv2R3Keygroup >= 0)
        {
            const int value = pendingEnv2R3Value;
            const int keygroupIndex = pendingEnv2R3Keygroup;

            pendingEnv2R3Value = -1;
            pendingEnv2R3Keygroup = -1;

            waitingForFilterReply = true;

            const int programIndex =
                loadedProgram.programNumber;

            juce::MessageManager::callAsync(
                [this, programIndex, keygroupIndex, value]()
                {
                    sysExSender.sendKeygroupHeaderByte(
                        programIndex,
                        keygroupIndex,
                        static_cast<int>(
                            KeygroupHeaderOffset::Env2::R3
                            ),
                        value
                    );
                }
            );

            return;
        }


        if (pendingEnv2L3Value >= 0 &&
            pendingEnv2L3Keygroup >= 0)
        {
            const int value = pendingEnv2L3Value;
            const int keygroupIndex = pendingEnv2L3Keygroup;

            pendingEnv2L3Value = -1;
            pendingEnv2L3Keygroup = -1;

            waitingForFilterReply = true;

            const int programIndex =
                loadedProgram.programNumber;

            juce::MessageManager::callAsync(
                [this, programIndex, keygroupIndex, value]()
                {
                    sysExSender.sendKeygroupHeaderByte(
                        programIndex,
                        keygroupIndex,
                        static_cast<int>(
                            KeygroupHeaderOffset::Env2::L3
                            ),
                        value
                    );
                }
            );

            return;
        }


        if (pendingEnv2R4Value >= 0 &&
            pendingEnv2R4Keygroup >= 0)
        {
            const int value = pendingEnv2R4Value;
            const int keygroupIndex = pendingEnv2R4Keygroup;

            pendingEnv2R4Value = -1;
            pendingEnv2R4Keygroup = -1;

            waitingForFilterReply = true;

            const int programIndex =
                loadedProgram.programNumber;

            juce::MessageManager::callAsync(
                [this, programIndex, keygroupIndex, value]()
                {
                    sysExSender.sendKeygroupHeaderByte(
                        programIndex,
                        keygroupIndex,
                        static_cast<int>(
                            KeygroupHeaderOffset::Env2::R4
                            ),
                        value
                    );
                }
            );

            return;
        }


        if (pendingEnv2L4Value >= 0 &&
            pendingEnv2L4Keygroup >= 0)
        {
            const int value = pendingEnv2L4Value;
            const int keygroupIndex = pendingEnv2L4Keygroup;

            pendingEnv2L4Value = -1;
            pendingEnv2L4Keygroup = -1;

            waitingForFilterReply = true;

            const int programIndex =
                loadedProgram.programNumber;

            juce::MessageManager::callAsync(
                [this, programIndex, keygroupIndex, value]()
                {
                    sysExSender.sendKeygroupHeaderByte(
                        programIndex,
                        keygroupIndex,
                        static_cast<int>(
                            KeygroupHeaderOffset::Env2::L4
                            ),
                        value
                    );
                }
            );

            return;
        }

        if (pendingEnv1AttackValue >= 0 &&
            pendingEnv1AttackKeygroup >= 0)
        {
            const int value =
                pendingEnv1AttackValue;

            const int keygroupIndex =
                pendingEnv1AttackKeygroup;

            pendingEnv1AttackValue = -1;
            pendingEnv1AttackKeygroup = -1;

            waitingForFilterReply = true;

            const int programIndex =
                loadedProgram.programNumber;

            juce::MessageManager::callAsync(
                [this, programIndex, keygroupIndex, value]()
                {
                    sysExSender.sendKeygroupHeaderByte(
                        programIndex,
                        keygroupIndex,
                        static_cast<int>(
                            KeygroupHeaderOffset::Env1::ATTACK
                            ),
                        value
                    );
                }
            );

            return;
        }

        if (pendingEnv1DecayValue >= 0 &&
            pendingEnv1DecayKeygroup >= 0)
        {
            const int value =
                pendingEnv1DecayValue;

            const int keygroupIndex =
                pendingEnv1DecayKeygroup;

            pendingEnv1DecayValue = -1;
            pendingEnv1DecayKeygroup = -1;

            waitingForFilterReply = true;

            const int programIndex =
                loadedProgram.programNumber;

            juce::MessageManager::callAsync(
                [this, programIndex, keygroupIndex, value]()
                {
                    sysExSender.sendKeygroupHeaderByte(
                        programIndex,
                        keygroupIndex,
                        static_cast<int>(
                            KeygroupHeaderOffset::Env1::DECAY
                            ),
                        value
                    );
                }
            );

            return;
        }


        if (pendingEnv1SustainValue >= 0 &&
            pendingEnv1SustainKeygroup >= 0)
        {
            const int value =
                pendingEnv1SustainValue;

            const int keygroupIndex =
                pendingEnv1SustainKeygroup;

            pendingEnv1SustainValue = -1;
            pendingEnv1SustainKeygroup = -1;

            waitingForFilterReply = true;

            const int programIndex =
                loadedProgram.programNumber;

            juce::MessageManager::callAsync(
                [this, programIndex, keygroupIndex, value]()
                {
                    sysExSender.sendKeygroupHeaderByte(
                        programIndex,
                        keygroupIndex,
                        static_cast<int>(
                            KeygroupHeaderOffset::Env1::SUSTAIN
                            ),
                        value
                    );
                }
            );

            return;
        }


        if (pendingEnv1ReleaseValue >= 0 &&
            pendingEnv1ReleaseKeygroup >= 0)
        {
            const int value =
                pendingEnv1ReleaseValue;

            const int keygroupIndex =
                pendingEnv1ReleaseKeygroup;

            pendingEnv1ReleaseValue = -1;
            pendingEnv1ReleaseKeygroup = -1;

            waitingForFilterReply = true;

            const int programIndex =
                loadedProgram.programNumber;

            juce::MessageManager::callAsync(
                [this, programIndex, keygroupIndex, value]()
                {
                    sysExSender.sendKeygroupHeaderByte(
                        programIndex,
                        keygroupIndex,
                        static_cast<int>(
                            KeygroupHeaderOffset::Env1::RELEASE
                            ),
                        value
                    );
                }
            );

            return;
        }


        return;
    }
}


void MainComponent::handleProgramHeaderResponse()
{


    auto decoded =
        decodeProgramHeader(programBuffer);

    DBG("=== ENTER handleProgramHeaderResponse ===");

    DBG(
        "PROGRAM HEADER MODSPITCH="
        + juce::String(
            static_cast<int8_t>(
                decoded[ProgramOffset::Mod::ModSPitch]
                )
        )
        + " MODVPITCH="
        + juce::String(
            static_cast<int8_t>(
                decoded[ProgramOffset::Mod::ModVPitch]
                )
        )
    );

    DBG(
        "PROGRAM L_PTCH OFFSET150 RAW="
        + juce::String(
            static_cast<int8_t>(
                decoded[150]
                )
        )
    );

    static std::vector<uint8_t> previousProgramData;

    DBG(
        "PROGRAM HEADER MODSPITCH="
        + juce::String(
            static_cast<int8_t>(
                decoded[ProgramOffset::Mod::ModSPitch]
                )
        )
        + " MODVPITCH="
        + juce::String(
            static_cast<int8_t>(
                decoded[ProgramOffset::Mod::ModVPitch]
                )
        )
    );

    if (!previousProgramData.empty()
        && previousProgramData.size() == decoded.size())
    {
        DBG("=== PROGRAM HEADER DIFF ===");

        for (size_t i = 0; i < decoded.size(); ++i)
        {
            if (decoded[i] != previousProgramData[i])
            {
                DBG(
                    "OFFSET "
                    + juce::String((int)i)
                    + ": "
                    + juce::String((int)previousProgramData[i])
                    + " -> "
                    + juce::String((int)decoded[i])
                );
            }
        }
    }

    previousProgramData = decoded;




    if (decoded.size() < 192)
    {
        DBG(
            "INVALID PROGRAM HEADER SIZE - IGNORING RESPONSE"
        );

        programBuffer.reset();
        return;
    }

    auto existingKeygroups =
        std::move(loadedProgram.keygroups);




    loadedProgram =
        ProgramParser::parse(decoded);

    //loadedProgram.programNumber =
    //    currentProgram;



    loadedProgram.keygroups =
        std::move(existingKeygroups);

    loadedProgram.rawData = decoded;

    programBuffer.reset();


    // ========================================
    // Program Editor �͏�ɍX�V
    // ========================================

    const Program programForUI =
        loadedProgram;

    juce::MessageManager::callAsync(
        [this, programForUI]()
        {


            programEditor.setProgram(
                programForUI
            );


        }
    );


    // ========================================
    // ���@�p�����[�^�ύX�ɂ��Ď擾�Ȃ�
    // �����͈�؍X�V���Ȃ�
    // ========================================

    DBG(
        "PROGRAM HEADER RESPONSE: programRefreshOnly="
        + juce::String(programRefreshOnly ? 1 : 0)
    );


    if (programRefreshOnly)
    {
        programRefreshOnly = false;

        DBG(
            "PROGRAM REFRESH ONLY - "
            "SKIP KEYGROUP RELOAD"
        );

        return;
    }


    // ========================================
    // ��������ʏ�� Program Load
    // ========================================

    totalKeygroups =
        loadedProgram.groups;

    loadingKeygroup = 0;

    const int mapKeygroupCount =
        totalKeygroups;

    juce::MessageManager::callAsync(
        [this, mapKeygroupCount]()
        {
            constexpr int rowHeightForMap = 32;
            constexpr int extraHeight = 64;

            const int contentHeight =
                juce::jmax(
                    200,
                    mapKeygroupCount
                    * rowHeightForMap
                    + extraHeight
                );

            keygroupMap.setSize(
                juce::jmax(
                    1,
                    keygroupMapViewport.getWidth() - 16
                ),
                contentHeight
            );


        }
    );




    // ========================================
    // Request first Keygroup
    // ========================================

    if (totalKeygroups > 0)
    {
        DBG(
            "PROGRAM HEADER RESPONSE: FULL LOAD -> SEND KG HEADER "
            + juce::String(loadingKeygroup)
        );

        DBG(
            "START FULL KG LOAD"
            " program=" + juce::String(loadedProgram.programNumber)
            + " groups=" + juce::String(totalKeygroups)
            + " refreshOnly="
            + juce::String(programRefreshOnly ? 1 : 0)
            + " refreshPending="
            + juce::String(programRefreshPending ? 1 : 0)
        );


        sysExSender.sendKGHeader(
            loadedProgram.programNumber,
            loadingKeygroup
        );
    }
}

void MainComponent::addKeygroup()
{
    if (waitingForFilterReply)
    {
        DBG("ADD KG BLOCKED - FILTER COMMAND IN PROGRESS");
        return;
    }

    if (loadedProgram.keygroups.empty())
    {
        DBG("ADD KG: NO KEYGROUPS");
        return;
    }

    if (pendingAddStage !=
        PendingAddStage::none)
    {
        DBG("ADD KG: ALREADY IN PROGRESS");
        return;
    }

    const int oldGroups =
        loadedProgram.groups;

    if (oldGroups >= 99)
    {
        DBG("ADD KG: MAX KEYGROUPS");
        return;
    }

    const int newKeygroupIndex =
        oldGroups;

    // ========================================
    // �Ō��KG���R�s�[���ĐVKG�����
    // ========================================

    if (currentKeygroup < 0 ||
        currentKeygroup >=
        static_cast<int>(
            loadedProgram.keygroups.size()
            ))
    {
        DBG("ADD KG: INVALID SOURCE KG");
        return;
    }

    const int sourceKeygroupIndex =
        currentKeygroup;

    Keygroup newKg =
        loadedProgram.keygroups[
            sourceKeygroupIndex
        ];

    auto encodedKg =
        KeygroupEncoder::encode(
            newKg
        );

    if (encodedKg.empty())
    {
        DBG("ADD KG: KG ENCODE FAILED");
        return;
    }

    // ========================================
    // Reply��ɕK�v�Ȃ̂ŕۑ�
    // ========================================

    pendingAddedKeygroup =
        newKg;

    pendingAddKeygroupIndex =
        newKeygroupIndex;

    pendingAddStage =
        PendingAddStage::
        waitingForKeygroupReply;


    // ========================================
    // STEP 1
    // KDATA��������
    // ========================================

    sysExSender.sendKeygroupData(
        loadedProgram.programNumber,
        newKeygroupIndex,
        encodedKg
    );
}

void MainComponent::handleKeygroupFullResponse()
{


    auto decoded =
        decodeKeygroupFull(programBuffer);


}

void MainComponent::handleRPDataResponse()
{
    DBG("RPDATA RESPONSE RECEIVED");

    std::vector<uint8_t> decoded;

    parseRPDATA(decoded);
}




void MainComponent::handleKeygroupHeaderResponse()
{
    DBG("ENTER CASE 0x2A");

    if (restoringKeygroupDisplay)
    {
        DBG("RESTORE KG DISPLAY RESPONSE");

        restoringKeygroupDisplay = false;

        if (!pendingSampleRequests.empty())
        {
            DBG(
                "RESTORE COMPLETE - START SAMPLE HEADERS"
                " pending="
                + juce::String(
                    (int)pendingSampleRequests.size()
                )
            );

            trySendNextSampleHeader();
        }
        else
        {
            DBG(
                "RESTORE COMPLETE - NO SAMPLE HEADERS NEEDED"
            );
        }

        return;
    }

    if (pendingAddProgramStage !=
        PendingAddProgramStage::none)
    {
        DBG(
            "ADD PROGRAM ACTIVE: IGNORE OLD KG HEADER RESPONSE"
        );
        return;
    }


    auto decoded =
        decodeKeygroupHeader(programBuffer);

    DBG(
        "decodeKeygroupHeader size = "
        + juce::String((int)decoded.size())
    );

    // 0x2A�ł�loadedProgram���X�V���Ȃ��B
    // ���S��Keygroup����0x09 KDATA�Ŏ擾����B

    const int requestProgram =
        loadedProgram.programNumber;

    const int requestKeygroup =
        loadingKeygroup;

    DBG(
        "QUEUE KDATA INDEX="
        + juce::String(requestKeygroup)
    );

    juce::MessageManager::callAsync(
        [this, requestProgram, requestKeygroup]()
        {
            DBG(
                "SEND KDATA INDEX="
                + juce::String(requestKeygroup)
            );

            sysExSender.sendKData(
                requestProgram,
                requestKeygroup
            );
        }
    );
}



int MainComponent::findSampleId(
    const juce::String& name)
{
    const auto target =
        name.trim();



    for (const auto& s : residentSamples)
    {
        const auto candidate =
            s.second.trim();



        if (candidate == target)
        {
            DBG(
                "MATCHED ID="
                + juce::String(s.first)
            );

            return s.first;
        }
    }



    return -1;
}

void MainComponent::resolveAllSampleIds()
{
    for (auto& kg : loadedProgram.keygroups)
    {
        for (auto& zone : kg.zones)
        {
            if (zone.sampleName.isEmpty())
                continue;

            int sampleIndex =
                findSampleId(zone.sampleName);


            if (sampleIndex >= 0)
            {
                zone.sampleId = sampleIndex;


            }
        }
    }
}



std::vector<uint8_t> MainComponent::decodeProgramHeader(
    const juce::MemoryBlock& data)
{
    std::vector<uint8_t> decoded;

    auto* p =
        (const uint8_t*)data.getData();

    size_t size = data.getSize();

    const int start = 11;


    for (size_t i = start; i + 1 < size; i += 2)
    {
        uint8_t low =
            p[i] & 0x0F;

        uint8_t high =
            p[i + 1] & 0x0F;


        decoded.push_back(
            (high << 4) | low
        );
    }


    return decoded;
}


void MainComponent::parseSLIST(
    const juce::MemoryBlock& data)
{
    const auto dataSize =
        static_cast<int>(data.getSize());



    // count��ǂނɂ͍Œ�6 bytes�K�v
    if (dataSize < 6)
    {
        DBG("SLIST ERROR: data too short");
        return;
    }

    auto* p =
        static_cast<const uint8_t*>(
            data.getData()
            );

    const int count =
        p[4] | (p[5] << 7);

    DBG("SAMPLE COUNT = "
        + juce::String(count));

    // 6-byte header + 12 bytes/sample
    const int requiredSize =
        6 + count * 12;



    if (requiredSize > dataSize)
    {
        DBG(
            "SLIST ERROR: incomplete data. "
            "required="
            + juce::String(requiredSize)
            + " actual="
            + juce::String(dataSize)
        );

        return;
    }

    residentSamples.clear();

    int offset = 6;

    for (int i = 0; i < count; ++i)
    {
        juce::String name;

        for (int j = 0; j < 12; ++j)
        {
            name += decodePlistChar(
                p[offset + j]
            );
        }

        name = name.trim();

        residentSamples[i] = name;

        DBG(
            juce::String(i)
            + " : "
            + name
        );

        offset += 12;
    }



    const auto samplesCopy =
        residentSamples;

    juce::MessageManager::callAsync(
        [this, samplesCopy]()
        {


            velocityZoneEditor.setResidentSamples(
                samplesCopy
            );


        }
    );
}




void MainComponent::saveDump(const juce::String& name)
{

    auto file = juce::File::getSpecialLocation(
        juce::File::userDesktopDirectory)
        .getChildFile(name + ".bin");

    file.replaceWithData(sysExBuffer.getData(),
        sysExBuffer.getSize());



}

void MainComponent::saveDecodedDump(const juce::String& name,
    const std::vector<uint8_t>& decoded)
{

    auto file = juce::File::getSpecialLocation(
        juce::File::userDesktopDirectory)
        .getChildFile(name + "_decoded.bin");

    file.replaceWithData(decoded.data(),
        decoded.size());



}

void MainComponent::compareDumps(const juce::File& fileA, const juce::File& fileB)
{


    juce::MemoryBlock dataA, dataB;

    if (!fileA.loadFileAsData(dataA))
    {
        DBG("Failed to load A");
        return;
    }

    if (!fileB.loadFileAsData(dataB))
    {
        DBG("Failed to load B");
        return;
    }




    const auto* a = (const uint8_t*)dataA.getData();
    const auto* b = (const uint8_t*)dataB.getData();

    const size_t size = juce::jmin(dataA.getSize(), dataB.getSize());

    DBG("=== CLUSTERED DIFF START ===");

    size_t lastDiff = (size_t)-1000;

    for (size_t i = 0; i < size; ++i)
    {
        if (a[i] != b[i])
        {
            if (i > lastDiff + 50)
            {
                DBG("---- new region ----");
            }

            DBG("Offset " + juce::String((int)i) +
                " : " +
                juce::String::toHexString((int)a[i]).paddedLeft('0', 2) +
                " -> " +
                juce::String::toHexString((int)b[i]).paddedLeft('0', 2));

            lastDiff = i;
        }
    }


}


void MainComponent::sendSampleHeader(int sampleId)
{
    DBG(
        "sendSampleHeader CALLED ID="
        + juce::String(sampleId)
    );

    if (sampleHeaders.find(sampleId)
        != sampleHeaders.end())
    {
        DBG(
            "SAMPLE HEADER ALREADY LOADED ID="
            + juce::String(sampleId)
        );

        return;
    }

    if (activeSampleHeaderRequestId == sampleId)
    {
        DBG(
            "SAMPLE HEADER ALREADY ACTIVE ID="
            + juce::String(sampleId)
        );

        return;
    }

    if (pendingSampleRequests.find(sampleId)
        != pendingSampleRequests.end())
    {
        DBG(
            "SAMPLE HEADER ALREADY QUEUED ID="
            + juce::String(sampleId)
            + " ACTIVE="
            + juce::String(activeSampleHeaderRequestId)
            + " QUEUE SIZE="
            + juce::String(
                (int)pendingSampleRequestOrder.size()
            )
        );

        for (int id : pendingSampleRequestOrder)
        {
            DBG(
                "  QUEUED ID="
                + juce::String(id)
            );
        }


        return;
    }

    pendingSampleRequests.insert(sampleId);

    pendingSampleRequestOrder.push_back(
        sampleId
    );

    trySendNextSampleHeader();
}

void MainComponent::trySendNextSampleHeader()
{
    DBG(
        "trySendNextSampleHeader ACTIVE="
        + juce::String(activeSampleHeaderRequestId)
        + " QUEUE SIZE="
        + juce::String(
            (int)pendingSampleRequestOrder.size()
        )
    );

    if (activeSampleHeaderRequestId >= 0)
    {
        DBG(
            "TRY SEND BLOCKED BY ACTIVE ID="
            + juce::String(activeSampleHeaderRequestId)
        );

        return;
    }

    if (pendingSampleRequestOrder.empty())
    {
        DBG("TRY SEND: QUEUE EMPTY");
        return;
    }

    const int sampleId =
        pendingSampleRequestOrder.front();

    pendingSampleRequestOrder.pop_front();

    activeSampleHeaderRequestId =
        sampleId;

    DBG(
        "REQUEST SAMPLE HEADER ID="
        + juce::String(sampleId)
    );

    sysExSender.sendSampleHeader(
        sampleId
    );
}






void MainComponent::parseRPDATA(const std::vector<uint8_t>& decoded)
{
    static int dumpIndex = 0;

    juce::File file =
        juce::File::getSpecialLocation(
            juce::File::userDesktopDirectory)
        .getChildFile(
            "decoded_" + juce::String(dumpIndex++) + ".bin");



    //auto decoded = decodeNibbleData(data);

    file.replaceWithData(
        decoded.data(),
        decoded.size()
    );



    juce::String s;

    for (int i = 0; i < 32 && i < decoded.size(); ++i)
    {
        s += juce::String::formatted("%02X ", decoded[i]);
    }

    DBG(s);

    for (int i = 0; i < 40; i++)
    {
        DBG(
            juce::String(i)
            + " : "
            + juce::String::toHexString(decoded[i])
        );
    }


    Program program = ProgramParser::parse(decoded);



}





void MainComponent::parsePLIST(const std::vector<uint8_t>& d)
{
    programList.clear();
    programCombo.clear(
        juce::dontSendNotification
    );

    int i = 4;

    if (d.size() < 6)
        return;

    const int count =
        (d[i] & 0x7F)
        | ((d[i + 1] & 0x7F) << 7);

    i += 2;



    for (int p = 0; p < count; ++p)
    {
        if (i + 12 > (int)d.size())
            break;

        std::string name;

        for (int n = 0; n < 12; ++n)
        {
            uint8_t v = d[i++];

            if (v == 0x00 || v == 0x0A)
                name += ' ';
            else
                name += decodePlistChar(v);
        }

        // trim
        while (!name.empty() && name.back() == ' ')
            name.pop_back();

        ProgramEntry entry;
        entry.index = p; // �� ���zindex�i���@�͖����j
        entry.name = name;

        programList.push_back(entry);

        // ComboBox�ɂ��ǉ�
        programCombo.addItem(
            juce::String(name),
            p + 1
        );

        DBG(juce::String(p) + " : " + name);
    }

    if (programCombo.getNumItems() > 0)
    {
        if (pendingProgramSelection >= 0)
        {
            if (pendingProgramSelection
                < programCombo.getNumItems())
            {
                const int comboId =
                    pendingProgramSelection + 1;

                pendingProgramSelection = -1;

                programCombo.setSelectedId(
                    comboId,
                    juce::sendNotification
                );
            }
            else
            {
                DBG(
                    "PLIST: WAITING FOR PROGRAM "
                    + juce::String(pendingProgramSelection)
                    + " CURRENT COUNT="
                    + juce::String(programCombo.getNumItems())
                );
            }
        }
        else
        {
            if (currentProgram < 0)
            {
                // Initial PLIST:
                // select and load Program 0
                programCombo.setSelectedId(
                    1,
                    juce::sendNotification
                );
            }
            else if (currentProgram < programCombo.getNumItems())
            {
                // PLIST refresh:
                // keep the currently loaded Program selected
                programCombo.setSelectedId(
                    currentProgram + 1,
                    juce::dontSendNotification
                );
            }
        }
    }

    listBox.updateContent();
    listBox.repaint();

    repaint();
}

uint8_t MainComponent::unpack7bit(const std::vector<uint8_t>& d, int& bitPos)
{
    int bytePos = bitPos / 7;
    int shift = bitPos % 7;

    if (bytePos >= (int)d.size())
        return 0;

    uint16_t value = d[bytePos];

    if (bytePos + 1 < (int)d.size())
        value |= (d[bytePos + 1] << 8);

    uint8_t result = (value >> shift) & 0x7F;

    bitPos += 7;

    return result;
}



std::vector<uint8_t> MainComponent::decodeNibbleData(const juce::MemoryBlock& data)
{


    auto* raw = (const uint8_t*)data.getData();

    juce::String s;

    for (int i = 0; i < 32 && i < data.getSize(); ++i)
    {
        s += juce::String::formatted("%02X ", raw[i]);
    }

    DBG(s);

    std::vector<uint8_t> decoded;

    const uint8_t* p = (const uint8_t*)data.getData();
    size_t size = data.getSize();

    const int start = 8;

    for (size_t i = start; i + 1< size; i +=2)
    {
        //decoded.push_back(p[i] & 0x7F);
        uint8_t high = p[i + 1] & 0x0F;
        uint8_t low = p[i] & 0x0F;

        uint8_t value = (high << 4) | low;
        decoded.push_back(value);
    }


    return decoded;
}

std::vector<uint8_t> MainComponent::decodeKeygroupNibbleData(
    const juce::MemoryBlock& data)
{
    std::vector<uint8_t> decoded;

    auto* p = (const uint8_t*)data.getData();

    const size_t start = 11;

    for (size_t i = start; i + 1 < data.getSize(); i += 2)
    {
        uint8_t low = p[i] & 0x0F;
        uint8_t high = p[i + 1] & 0x0F;

        decoded.push_back((high << 4) | low);
    }

    return decoded;
}

void MainComponent::loadProgram(
    int programIndex)
{


    if (programIndex < 0)
        return;

    // ==============================
    // Sample Header request state reset
    // ==============================
    pendingSampleRequests.clear();
    pendingSampleRequestOrder.clear();
    activeSampleHeaderRequestId = -1;

    currentProgram = programIndex;

    currentProgramIndex =
        programIndex;

    programCombo.setSelectedItemIndex(
        programIndex,
        juce::dontSendNotification
    );


    currentKeygroup = 0;
    currentZone = 0;

    loadingKeygroup = 0;

 /*   loadedProgram.programNumber =
        programIndex;*/

    loadedProgram.keygroups.clear();

    sysExSender.sendRSLIST();



    // Program���[�h�J�n
    sysExSender.sendProgramHeader(
        programIndex
    );



}


std::vector<uint8_t> MainComponent::decodeKeygroupHeader(
    const juce::MemoryBlock& data)
{
    std::vector<uint8_t> decoded;

    auto* p = (const uint8_t*)data.getData();


    // Keygroup response header is 11 bytes
    const int start = 11;

    for (size_t i = start; i + 1 < data.getSize(); i += 2)
    {
        uint8_t low = p[i] & 0x0F;
        uint8_t high = p[i + 1] & 0x0F;

        decoded.push_back(
            (high << 4) | low
        );
    }



    return decoded;
}



std::vector<uint8_t> MainComponent::decodeKData(
    const uint8_t* data,
    size_t size)
{
    std::vector<uint8_t> decoded;


    // SysEx header
    // F0��message.getSysExData()�ɂ͊܂܂�Ȃ�
    // 47 cc 09 48 pp pp kk �̌ォ��data�J�n

    const int start = 7;


    for (size_t i = start; i + 1 < size; i += 2)
    {
        uint8_t low =
            data[i] & 0x0F;

        uint8_t high =
            data[i + 1] & 0x0F;


        decoded.push_back(
            (high << 4) | low
        );
    }


    return decoded;
}


std::vector<uint8_t> MainComponent::decodeKeygroupFull(
    const juce::MemoryBlock& data)
{
    std::vector<uint8_t> decoded;


    auto* p =
        (const uint8_t*)data.getData();


    size_t size =
        data.getSize();


    const int start = 11;


    for (size_t i = start; i + 1 < size; i += 2)
    {
        uint8_t low =
            p[i] & 0x0F;

        uint8_t high =
            p[i + 1] & 0x0F;


        decoded.push_back(
            (high << 4) | low
        );
    }


    return decoded;
}

std::vector<uint8_t> MainComponent::decodeSampleHeader(
    const juce::MemoryBlock& data)
{


    std::vector<uint8_t> decoded;


    auto* p =
        (const uint8_t*)data.getData();


    size_t size =
        data.getSize();


    const int start = 11;


    for (size_t i = start; i + 1 < size; i += 2)
    {
        uint8_t low =
            p[i] & 0x0F;

        uint8_t high =
            p[i + 1] & 0x0F;


        decoded.push_back(
            (high << 4) | low
        );
    }


    return decoded;
}


std::string decodeName(const uint8_t* p)
{
    std::string s;
    for (int i = 0; i < 12; i++)
    {
        /*s += decodeAkaiChar(p[i + 4]);*/
        s += decodeAkaiChar(p[i + 3]);
    }
    return s;
}

char MainComponent::decodePlistChar(uint8_t v)
{
    if (v <= 9) return '0' + v;
    if (v == 10) return ' ';
    if (v >= 11 && v <= 36) return 'A' + (v - 11);
    if (v == 37) return '#';
    if (v == 38) return '+';
    if (v == 39) return '-';
    if (v == 40) return '.';

    return '?';
}



void MainComponent::parseProgram(const std::vector<uint8_t>& decoded)
{
    static int dumpIndex = 0;

    juce::File file =
        juce::File::getSpecialLocation(
            juce::File::userDesktopDirectory)
        .getChildFile(
            "Program_" + juce::String(dumpIndex++) + ".bin"
        );

    file.replaceWithData(
        decoded.data(),
        decoded.size()
    );



    Program program = ProgramParser::parse(decoded);




}




void MainComponent::saveRawDump(const juce::String& name)
{
    auto file = juce::File::getSpecialLocation(
        juce::File::userDesktopDirectory)
        .getChildFile(name + "_raw.bin");

    file.replaceWithData(sysExBuffer.getData(),
        sysExBuffer.getSize());


}

void MainComponent::saveRawRPDATA(const juce::String& name,
    const juce::MemoryBlock& data)
{
    auto file = juce::File::getSpecialLocation(
        juce::File::userDesktopDirectory)
        .getChildFile(name + "_raw.bin");

    file.replaceWithData(data.getData(), data.getSize());


}



int MainComponent::getNumRows()
{
    return (int)programList.size();
}

void MainComponent::paintListBoxItem(int rowNumber,
    juce::Graphics& g,
    int width, int height,
    bool rowIsSelected)
{
    if (rowNumber >= programList.size())
        return;

    auto& p = programList[rowNumber];

    if (rowIsSelected)
        g.fillAll(juce::Colours::darkblue);

    g.setColour(juce::Colours::white);
    g.drawText(juce::String(p.index) + " " + p.name,
        5, 0, width, height,
        juce::Justification::centredLeft);
}


std::string MainComponent::trimRightSpaces(std::string s)
{
    while (!s.empty() && s.back() == ' ')
        s.pop_back();
    return s;
}

void MainComponent::listBoxItemClicked(
    int row,
    const juce::MouseEvent&)
{

    if (row < 0 || row >= programList.size())
    {
        return;
    }


    currentProgramIndex =
        programList[row].index;



    DBG(
        "RESIDENT PROGRAM SELECTED"
        " ROW="
        + juce::String(row)
        + " INDEX="
        + juce::String(currentProgramIndex)
    );





    sysExSender.sendProgramHeader(currentProgramIndex);
}

void MainComponent::handleNoteOn(
    juce::MidiKeyboardState*,
    int,
    int midiNoteNumber,
    float)
{

    DBG(
        "HANDLE NOTE ON FROM MIDI KEYBOARD STATE NOTE="
        + juce::String(midiNoteNumber)
    );

    if (!keyboardToggle.getToggleState())
        return;

    const int velocity =
        (int)keyboardVelocitySlider.getValue();

    sysExSender.sendNoteOn(
        midiNoteNumber,
        velocity,
        16
    );
}

void MainComponent::handleNoteOff(
    juce::MidiKeyboardState*,
    int,
    int midiNoteNumber,
    float)
{
    sysExSender.sendNoteOff(
        midiNoteNumber,
        16
    );
}


void MainComponent::injectTestKeygroupHeaderByte(
    int programNumber,
    int keygroupIndex,
    int offset,
    uint8_t value)
{
    DBG(
        "INJECT TEST KG HEADER BYTE"
        " PROGRAM=" + juce::String(programNumber)
        + " KG=" + juce::String(keygroupIndex)
        + " OFFSET=" + juce::String(offset)
        + " VALUE=" + juce::String((int)value)
    );
    
    
    uint8_t data[] =
    {
        0x47,
        0x00,
        0x2A,
        0x48,

        static_cast<uint8_t>(programNumber & 0x7F),
        static_cast<uint8_t>((programNumber >> 7) & 0x7F),

        static_cast<uint8_t>(keygroupIndex & 0x7F),

        static_cast<uint8_t>(offset & 0x7F),
        static_cast<uint8_t>((offset >> 7) & 0x7F),

        0x01,
        0x00,

        static_cast<uint8_t>(value & 0x0F),
        static_cast<uint8_t>((value >> 4) & 0x0F)
    };

    auto message =
        juce::MidiMessage::createSysExMessage(
            data,
            sizeof(data)
        );

    processIncomingSysEx(message);
}

bool MainComponent::keyPressed(
    const juce::KeyPress& key,
    juce::Component*)
{
    DBG(
        "PC KEY PRESSED: "
        + key.getTextDescription()
    );

    keyGroupEditor.suspendFilterRealtime();

    const bool handled =
        keyboardComponent.keyPressed(key);

    DBG(
        "keyboardComponent.keyPressed RESULT="
        + juce::String(handled ? 1 : 0)
    );

    return handled;
}

bool MainComponent::keyStateChanged(
    bool isKeyDown,
    juce::Component*)
{
    DBG(
        "PC KEY STATE CHANGED isKeyDown="
        + juce::String(isKeyDown ? 1 : 0)
    );

    return keyboardComponent.keyStateChanged(isKeyDown);
}

void MainComponent::saveProject()
{
    auto chooser = std::make_shared<juce::FileChooser>(
        "Save S3000XL Project",
        juce::File{},
        "*.json"
    );

    chooser->launchAsync(
        juce::FileBrowserComponent::saveMode
        | juce::FileBrowserComponent::canSelectFiles,
        [this, chooser](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();

            if (file == juce::File{})
                return;

            juce::DynamicObject::Ptr root =
                new juce::DynamicObject();

            root->setProperty(
                "format",
                "S3000XL Editor Project"
            );

            root->setProperty(
                "version",
                1
            );

            juce::DynamicObject::Ptr program =
                new juce::DynamicObject();

            program->setProperty(
                "programNumber",
                loadedProgram.programNumber
            );

            program->setProperty(
                "name",
                juce::String(loadedProgram.name)
            );

            program->setProperty(
                "midiChannel",
                loadedProgram.midiChannel
            );

            program->setProperty(
                "polyphony",
                loadedProgram.polyphony
            );

            program->setProperty(
                "priority",
                loadedProgram.priority
            );

            program->setProperty(
                "playLow",
                loadedProgram.playLow
            );

            program->setProperty(
                "playHigh",
                loadedProgram.playHigh
            );

            program->setProperty(
                "pan",
                loadedProgram.pan
            );

            program->setProperty(
                "loudness",
                loadedProgram.loudness
            );

            program->setProperty("output", loadedProgram.output);
            program->setProperty("stereoLevel", loadedProgram.stereoLevel);

            program->setProperty("lfo1Rate", loadedProgram.lfo1Rate);
            program->setProperty("lfo1Depth", loadedProgram.lfo1Depth);
            program->setProperty("lfo1Delay", loadedProgram.lfo1Delay);

            program->setProperty("lfo2Rate", loadedProgram.lfo2Rate);
            program->setProperty("lfo2Depth", loadedProgram.lfo2Depth);
            program->setProperty("lfo2Delay", loadedProgram.lfo2Delay);

            program->setProperty("modWheelDepth", loadedProgram.modWheelDepth);
            program->setProperty("pressureDepth", loadedProgram.pressureDepth);
            program->setProperty("velocityDepth", loadedProgram.velocityDepth);

            program->setProperty(
                "velocityLoudness",
                loadedProgram.velocityLoudness
            );

            program->setProperty("bendUp", loadedProgram.bendUp);
            program->setProperty("pressurePitch", loadedProgram.pressurePitch);

            program->setProperty(
                "keygroupCrossfade",
                loadedProgram.keygroupCrossfade
            );

            program->setProperty("groups", loadedProgram.groups);
            program->setProperty("tune", loadedProgram.tune);

            program->setProperty(
                "individualOutputLevel",
                loadedProgram.individualOutputLevel
            );

            program->setProperty("legato", loadedProgram.legato);
            program->setProperty("bendDown", loadedProgram.bendDown);
            program->setProperty("bendMode", loadedProgram.bendMode);
            program->setProperty("transpose", loadedProgram.transpose);

            program->setProperty(
                "lfo1Desync",
                loadedProgram.lfo1Desync
            );

            program->setProperty(
                "voiceAssign",
                loadedProgram.voiceAssign
            );

            program->setProperty(
                "softLoudness",
                loadedProgram.softLoudness
            );

            program->setProperty(
                "softAttack",
                loadedProgram.softAttack
            );

            program->setProperty(
                "softFilter",
                loadedProgram.softFilter
            );

            // =========================
// Portamento
// =========================

            program->setProperty(
                "portamentoTime",
                loadedProgram.portamentoTime
            );

            program->setProperty(
                "portamentoType",
                loadedProgram.portamentoType
            );

            program->setProperty(
                "portamentoEnabled",
                loadedProgram.portamentoEnabled
            );

            program->setProperty("modSPan1", loadedProgram.modSPan1);
            program->setProperty("modSPan2", loadedProgram.modSPan2);
            program->setProperty("modSPan3", loadedProgram.modSPan3);

            program->setProperty("modSAmp1", loadedProgram.modSAmp1);
            program->setProperty("modSAmp2", loadedProgram.modSAmp2);
            program->setProperty("modSAmp3", loadedProgram.modSAmp3);

            program->setProperty(
                "modSLfo1Rate",
                loadedProgram.modSLfo1Rate
            );

            program->setProperty(
                "modSLfo1Depth",
                loadedProgram.modSLfo1Depth
            );

            program->setProperty(
                "modSLfo1Delay",
                loadedProgram.modSLfo1Delay
            );

            program->setProperty(
                "modSFilter1",
                loadedProgram.modSFilter1
            );

            program->setProperty(
                "modSFilter2",
                loadedProgram.modSFilter2
            );

            program->setProperty(
                "modSFilter3",
                loadedProgram.modSFilter3
            );

            program->setProperty(
                "modSPitch",
                loadedProgram.modSPitch
            );

            program->setProperty(
                "modVPitch",
                loadedProgram.modVPitch
            );

            program->setProperty("modVPan1", loadedProgram.modVPan1);
            program->setProperty("modVPan2", loadedProgram.modVPan2);
            program->setProperty("modVPan3", loadedProgram.modVPan3);

            program->setProperty("modVAmp1", loadedProgram.modVAmp1);
            program->setProperty("modVAmp2", loadedProgram.modVAmp2);

            program->setProperty(
                "modVLfo1Rate",
                loadedProgram.modVLfo1Rate
            );

            program->setProperty(
                "modVLfo1Depth",
                loadedProgram.modVLfo1Depth
            );

            program->setProperty(
                "modVLfo1Delay",
                loadedProgram.modVLfo1Delay
            );

            program->setProperty("lfo1Wave", loadedProgram.lfo1Wave);
            program->setProperty("lfo2Wave", loadedProgram.lfo2Wave);

            juce::Array<juce::var> temperament;

            for (const auto value : loadedProgram.temperament)
            {
                temperament.add(
                    static_cast<int>(value)
                );
            }

            program->setProperty(
                "temperament",
                temperament
            );

            root->setProperty(
                "program",
                juce::var(program.get())
            );

            juce::Array<juce::var> keygroups;

            for (const auto& kg : loadedProgram.keygroups)
            {
                juce::DynamicObject::Ptr keygroup =
                    new juce::DynamicObject();

                keygroup->setProperty(
                    "id",
                    static_cast<int>(kg.id)
                );

                keygroup->setProperty(
                    "nextAddress",
                    static_cast<int>(kg.nextAddress)
                );

                keygroup->setProperty(
                    "filterVelocityToFreq",
                    kg.filter.velocityToFreq
                );

                keygroup->setProperty(
                    "filterPressureToFreq",
                    kg.filter.pressureToFreq
                );

                keygroup->setProperty(
                    "filterEnvelopeToFreq",
                    kg.filter.envelopeToFreq
                );

                keygroup->setProperty(
                    "lowNote",
                    kg.lowNote
                );

                keygroup->setProperty(
                    "highNote",
                    kg.highNote
                );

                keygroup->setProperty(
                    "tune",
                    kg.tune
                );

                keygroup->setProperty(
                    "filterFreq",
                    kg.filter.freq
                );

                keygroup->setProperty(
                    "filterKeyFollow",
                    kg.filter.keyFollow
                );

                keygroup->setProperty(
                    "filterResonance",
                    kg.filter.resonance
                );

                keygroup->setProperty(
                    "lfo1Pitch",
                    kg.lfo1Pitch
                );

                keygroup->setProperty(
                    "modVPitch",
                    kg.modVPitch
                );

                keygroup->setProperty(
                    "modFilter1",
                    kg.modFilter1
                );

                keygroup->setProperty(
                    "modFilter2",
                    kg.modFilter2
                );

                keygroup->setProperty(
                    "modFilter3",
                    kg.modFilter3
                );

                juce::DynamicObject::Ptr env1 =
                    new juce::DynamicObject();

                env1->setProperty(
                    "attack",
                    kg.env1.attack
                );

                env1->setProperty(
                    "decay",
                    kg.env1.decay
                );

                env1->setProperty(
                    "sustain",
                    kg.env1.sustain
                );

                env1->setProperty(
                    "release",
                    kg.env1.release
                );

                keygroup->setProperty(
                    "env1",
                    juce::var(env1.get())
                );


                juce::DynamicObject::Ptr env2 =
                    new juce::DynamicObject();

                env2->setProperty(
                    "r1",
                    kg.env2.r1
                );

                env2->setProperty(
                    "l1",
                    kg.env2.l1
                );

                env2->setProperty(
                    "r2",
                    kg.env2.r2
                );

                env2->setProperty(
                    "l2",
                    kg.env2.l2
                );

                env2->setProperty(
                    "r3",
                    kg.env2.r3
                );

                env2->setProperty(
                    "l3",
                    kg.env2.l3
                );

                env2->setProperty(
                    "r4",
                    kg.env2.r4
                );

                env2->setProperty(
                    "l4",
                    kg.env2.l4
                );

                env2->setProperty(
                    "velAttack",
                    kg.env2.velAttack
                );

                env2->setProperty(
                    "velRelease",
                    kg.env2.velRelease
                );

                env2->setProperty(
                    "noteOffRelease",
                    kg.env2.noteOffRelease
                );

                env2->setProperty(
                    "keyTracking",
                    kg.env2.keyTracking
                );

                keygroup->setProperty(
                    "env2",
                    juce::var(env2.get())
                );

                juce::DynamicObject::Ptr playback =
                    new juce::DynamicObject();

                playback->setProperty(
                    "kbeat",
                    kg.playback.kbeat
                );

                playback->setProperty(
                    "ahold",
                    static_cast<int>(kg.playback.ahold)
                );

                playback->setProperty(
                    "constantPitch",
                    static_cast<int>(kg.playback.constantPitch)
                );

                playback->setProperty(
                    "keygroupXfade",
                    kg.playback.keygroupXfade
                );

                keygroup->setProperty(
                    "playback",
                    juce::var(playback.get())
                );


                juce::DynamicObject::Ptr velocity =
                    new juce::DynamicObject();

                velocity->setProperty(
                    "vAtt2",
                    static_cast<int>(kg.velocity.vAtt2)
                );

                velocity->setProperty(
                    "vRel2",
                    static_cast<int>(kg.velocity.vRel2)
                );

                velocity->setProperty(
                    "oRel2",
                    static_cast<int>(kg.velocity.oRel2)
                );

                velocity->setProperty(
                    "kDar2",
                    static_cast<int>(kg.velocity.kDar2)
                );

                velocity->setProperty(
                    "vEnv2",
                    static_cast<int>(kg.velocity.vEnv2)
                );

                velocity->setProperty(
                    "ePtch",
                    static_cast<int>(kg.velocity.ePtch)
                );

                velocity->setProperty(
                    "vxFade",
                    static_cast<int>(kg.velocity.vxFade)
                );

                velocity->setProperty(
                    "vZones",
                    static_cast<int>(kg.velocity.vZones)
                );

                velocity->setProperty(
                    "lkxf",
                    static_cast<int>(kg.velocity.lkxf)
                );

                velocity->setProperty(
                    "rkxf",
                    static_cast<int>(kg.velocity.rkxf)
                );

                keygroup->setProperty(
                    "velocity",
                    juce::var(velocity.get())
                );


                juce::Array<juce::var> zones;

                for (const auto& zone : kg.zones)
                {
                    juce::DynamicObject::Ptr zoneObject =
                        new juce::DynamicObject();

                    zoneObject->setProperty(
                        "sampleName",
                        zone.sampleName
                    );

                    zoneObject->setProperty(
                        "sampleId",
                        zone.sampleId
                    );

                    zoneObject->setProperty(
                        "lowVel",
                        static_cast<int>(zone.lowVel)
                    );

                    zoneObject->setProperty(
                        "highVel",
                        static_cast<int>(zone.highVel)
                    );

                    zoneObject->setProperty(
                        "semitone",
                        zone.semitone
                    );

                    zoneObject->setProperty(
                        "fineTuneRaw",
                        zone.fineTuneRaw
                    );

                    zoneObject->setProperty(
                        "loudness",
                        static_cast<int>(zone.loudness)
                    );

                    zoneObject->setProperty(
                        "filterFreq",
                        static_cast<int>(zone.filterFreq)
                    );

                    zoneObject->setProperty(
                        "pan",
                        static_cast<int>(zone.pan)
                    );

                    zoneObject->setProperty(
                        "playMode",
                        static_cast<int>(zone.playMode)
                    );

                    zoneObject->setProperty(
                        "lowVelXFade",
                        static_cast<int>(zone.lowVelXFade)
                    );

                    zoneObject->setProperty(
                        "highVelXFade",
                        static_cast<int>(zone.highVelXFade)
                    );

                    zoneObject->setProperty(
                        "constantPitch",
                        zone.constantPitch
                    );

                    zones.add(
                        juce::var(zoneObject.get())
                    );
                }

                keygroup->setProperty(
                    "zones",
                    zones
                );

                keygroups.add(
                    juce::var(keygroup.get())
                );
            }

            root->setProperty(
                "keygroups",
                keygroups
            );

            juce::Array<juce::var> sampleHeadersArray;

            for (const auto& [sampleId, header] : sampleHeaders)
            {
                juce::DynamicObject::Ptr sampleHeaderObject =
                    new juce::DynamicObject();

                sampleHeaderObject->setProperty(
                    "sampleId",
                    sampleId
                );

                sampleHeaderObject->setProperty(
                    "id",
                    header.id
                );

                sampleHeaderObject->setProperty(
                    "bandwidth",
                    header.bandwidth
                );

                sampleHeaderObject->setProperty(
                    "originalPitch",
                    header.originalPitch
                );

                sampleHeaderObject->setProperty(
                    "name",
                    header.name
                );

                sampleHeaderObject->setProperty(
                    "sampleRateValid",
                    header.sampleRateValid
                );

                sampleHeaderObject->setProperty(
                    "numLoops",
                    header.numLoops
                );

                sampleHeaderObject->setProperty(
                    "activeLoop",
                    header.activeLoop
                );

                sampleHeaderObject->setProperty(
                    "highestLoop",
                    header.highestLoop
                );

                sampleHeaderObject->setProperty(
                    "playType",
                    header.playType
                );

                sampleHeaderObject->setProperty(
                    "tune",
                    header.tune
                );

                sampleHeaderObject->setProperty(
                    "location",
                    static_cast<juce::int64>(header.location)
                );

                sampleHeaderObject->setProperty(
                    "length",
                    static_cast<juce::int64>(header.length)
                );

                sampleHeaderObject->setProperty(
                    "start",
                    static_cast<juce::int64>(header.start)
                );

                sampleHeaderObject->setProperty(
                    "end",
                    static_cast<juce::int64>(header.end)
                );

                sampleHeaderObject->setProperty(
                    "spare",
                    static_cast<int>(header.spare)
                );

                sampleHeaderObject->setProperty(
                    "waveComment",
                    static_cast<int>(header.waveComment)
                );

                sampleHeaderObject->setProperty(
                    "stereoPartner",
                    static_cast<int>(header.stereoPartner)
                );

                sampleHeaderObject->setProperty(
                    "sampleRate",
                    static_cast<int>(header.sampleRate)
                );

                sampleHeaderObject->setProperty(
                    "holdLoopTune",
                    static_cast<int>(header.holdLoopTune)
                );

                juce::Array<juce::var> loops;

                for (const auto& loop : header.loops)
                {
                    juce::DynamicObject::Ptr loopObject =
                        new juce::DynamicObject();

                    loopObject->setProperty(
                        "position",
                        static_cast<juce::int64>(loop.position)
                    );

                    loopObject->setProperty(
                        "length",
                        loop.length
                    );

                    loopObject->setProperty(
                        "dwell",
                        static_cast<int>(loop.dwell)
                    );

                    loopObject->setProperty(
                        "relativeFactors",
                        static_cast<juce::int64>(loop.relativeFactors)
                    );

                    loops.add(
                        juce::var(loopObject.get())
                    );
                }

                sampleHeaderObject->setProperty(
                    "loops",
                    loops
                );

                sampleHeadersArray.add(
                    juce::var(sampleHeaderObject.get())
                );
            }

            root->setProperty(
                "sampleHeaders",
                sampleHeadersArray
            );

            const juce::String json =
                juce::JSON::toString(
                    juce::var(root.get()),
                    true
                );

            file.replaceWithText(json);

            DBG(
                "PROJECT SAVED = "
                + file.getFullPathName()
            );
        }
    );
}

void MainComponent::loadProject()
{
    auto chooser = std::make_shared<juce::FileChooser>(
        "Load S3000XL Project",
        juce::File{},
        "*.json"
    );

    chooser->launchAsync(
        juce::FileBrowserComponent::openMode
        | juce::FileBrowserComponent::canSelectFiles,
        [this, chooser](const juce::FileChooser& fc)
        {
            const auto file = fc.getResult();

            if (file == juce::File{})
                return;

            const juce::String jsonText =
                file.loadFileAsString();

            const juce::var json =
                juce::JSON::parse(jsonText);

            if (json.isVoid())
            {
                DBG("PROJECT LOAD FAILED: INVALID JSON");
                return;
            }

            auto* root =
                json.getDynamicObject();

            if (root == nullptr)
            {
                DBG("PROJECT LOAD FAILED: ROOT IS NOT OBJECT");
                return;
            }

            DBG(
                "PROJECT LOADED = "
                + file.getFullPathName()
            );

            DBG(
                "FORMAT = "
                + root->getProperty("format").toString()
            );

            DBG(
                "VERSION = "
                + root->getProperty("version").toString()
            );

            auto programVar =
                root->getProperty("program");

            auto* programObject =
                programVar.getDynamicObject();

            if (programObject == nullptr)
            {
                DBG("PROJECT LOAD FAILED: PROGRAM IS NOT OBJECT");
                return;
            }

            loadedProgram.programNumber =
                static_cast<int>(
                    programObject->getProperty("programNumber")
                    );

            loadedProgram.name =
                programObject->getProperty("name")
                .toString()
                .toStdString();

            loadedProgram.midiChannel =
                static_cast<int>(
                    programObject->getProperty("midiChannel")
                    );

            loadedProgram.polyphony =
                static_cast<int>(
                    programObject->getProperty("polyphony")
                    );

            loadedProgram.priority =
                static_cast<int>(
                    programObject->getProperty("priority")
                    );

            loadedProgram.playLow =
                static_cast<int>(
                    programObject->getProperty("playLow")
                    );

            loadedProgram.playHigh =
                static_cast<int>(
                    programObject->getProperty("playHigh")
                    );

            loadedProgram.pan =
                static_cast<int>(
                    programObject->getProperty("pan")
                    );

            loadedProgram.loudness =
                static_cast<int>(
                    programObject->getProperty("loudness")
                    );

            loadedProgram.output =
                static_cast<int>(
                    programObject->getProperty("output")
                    );

            loadedProgram.stereoLevel =
                static_cast<int>(
                    programObject->getProperty("stereoLevel")
                    );

            loadedProgram.lfo1Rate =
                static_cast<int>(
                    programObject->getProperty("lfo1Rate")
                    );

            loadedProgram.lfo1Depth =
                static_cast<int>(
                    programObject->getProperty("lfo1Depth")
                    );

            loadedProgram.lfo1Delay =
                static_cast<int>(
                    programObject->getProperty("lfo1Delay")
                    );

            loadedProgram.lfo2Rate =
                static_cast<int>(
                    programObject->getProperty("lfo2Rate")
                    );

            loadedProgram.lfo2Depth =
                static_cast<int>(
                    programObject->getProperty("lfo2Depth")
                    );

            loadedProgram.lfo2Delay =
                static_cast<int>(
                    programObject->getProperty("lfo2Delay")
                    );

            loadedProgram.modWheelDepth =
                static_cast<int>(
                    programObject->getProperty("modWheelDepth")
                    );

            loadedProgram.pressureDepth =
                static_cast<int>(
                    programObject->getProperty("pressureDepth")
                    );

            loadedProgram.velocityDepth =
                static_cast<int>(
                    programObject->getProperty("velocityDepth")
                    );

            loadedProgram.velocityLoudness =
                static_cast<int>(
                    programObject->getProperty("velocityLoudness")
                    );

            loadedProgram.bendUp =
                static_cast<int>(
                    programObject->getProperty("bendUp")
                    );

            loadedProgram.pressurePitch =
                static_cast<int>(
                    programObject->getProperty("pressurePitch")
                    );

            loadedProgram.keygroupCrossfade =
                static_cast<bool>(
                    programObject->getProperty("keygroupCrossfade")
                    );

            loadedProgram.tune =
                static_cast<double>(
                    programObject->getProperty("tune")
                    );

            loadedProgram.individualOutputLevel =
                static_cast<int>(
                    programObject->getProperty("individualOutputLevel")
                    );

            loadedProgram.legato =
                static_cast<bool>(
                    programObject->getProperty("legato")
                    );

            loadedProgram.bendDown =
                static_cast<int>(
                    programObject->getProperty("bendDown")
                    );

            loadedProgram.bendMode =
                static_cast<int>(
                    programObject->getProperty("bendMode")
                    );

            loadedProgram.transpose =
                static_cast<int>(
                    programObject->getProperty("transpose")
                    );

            loadedProgram.lfo1Desync =
                static_cast<bool>(
                    programObject->getProperty("lfo1Desync")
                    );

            loadedProgram.voiceAssign =
                static_cast<int>(
                    programObject->getProperty("voiceAssign")
                    );

            loadedProgram.softLoudness =
                static_cast<int>(
                    programObject->getProperty("softLoudness")
                    );

            loadedProgram.softAttack =
                static_cast<int>(
                    programObject->getProperty("softAttack")
                    );

            loadedProgram.softFilter =
                static_cast<int>(
                    programObject->getProperty("softFilter")
                    );

            // =========================
// Portamento
// =========================

            loadedProgram.portamentoTime =
                static_cast<int>(
                    programObject->getProperty("portamentoTime")
                    );

            loadedProgram.portamentoType =
                static_cast<int>(
                    programObject->getProperty("portamentoType")
                    );

            loadedProgram.portamentoEnabled =
                static_cast<bool>(
                    programObject->getProperty("portamentoEnabled")
                    );

            loadedProgram.modSPan1 =
                static_cast<int>(
                    programObject->getProperty("modSPan1")
                    );

            loadedProgram.modSPan2 =
                static_cast<int>(
                    programObject->getProperty("modSPan2")
                    );

            loadedProgram.modSPan3 =
                static_cast<int>(
                    programObject->getProperty("modSPan3")
                    );

            loadedProgram.modSAmp1 =
                static_cast<int>(
                    programObject->getProperty("modSAmp1")
                    );

            loadedProgram.modSAmp2 =
                static_cast<int>(
                    programObject->getProperty("modSAmp2")
                    );

            loadedProgram.modSAmp3 =
                static_cast<int>(
                    programObject->getProperty("modSAmp3")
                    );

            loadedProgram.modSLfo1Rate =
                static_cast<int>(
                    programObject->getProperty("modSLfo1Rate")
                    );

            loadedProgram.modSLfo1Depth =
                static_cast<int>(
                    programObject->getProperty("modSLfo1Depth")
                    );

            loadedProgram.modSLfo1Delay =
                static_cast<int>(
                    programObject->getProperty("modSLfo1Delay")
                    );

            loadedProgram.modSFilter1 =
                static_cast<int>(
                    programObject->getProperty("modSFilter1")
                    );

            loadedProgram.modSFilter2 =
                static_cast<int>(
                    programObject->getProperty("modSFilter2")
                    );

            loadedProgram.modSFilter3 =
                static_cast<int>(
                    programObject->getProperty("modSFilter3")
                    );

            loadedProgram.modSPitch =
                static_cast<int>(
                    programObject->getProperty("modSPitch")
                    );

            loadedProgram.modVPitch =
                static_cast<int>(
                    programObject->getProperty("modVPitch")
                    );

            loadedProgram.modVPan1 =
                static_cast<int>(
                    programObject->getProperty("modVPan1")
                    );

            loadedProgram.modVPan2 =
                static_cast<int>(
                    programObject->getProperty("modVPan2")
                    );

            loadedProgram.modVPan3 =
                static_cast<int>(
                    programObject->getProperty("modVPan3")
                    );

            loadedProgram.modVAmp1 =
                static_cast<int>(
                    programObject->getProperty("modVAmp1")
                    );

            loadedProgram.modVAmp2 =
                static_cast<int>(
                    programObject->getProperty("modVAmp2")
                    );

            loadedProgram.modVLfo1Rate =
                static_cast<int>(
                    programObject->getProperty("modVLfo1Rate")
                    );

            loadedProgram.modVLfo1Depth =
                static_cast<int>(
                    programObject->getProperty("modVLfo1Depth")
                    );

            loadedProgram.modVLfo1Delay =
                static_cast<int>(
                    programObject->getProperty("modVLfo1Delay")
                    );

            loadedProgram.lfo1Wave =
                static_cast<int>(
                    programObject->getProperty("lfo1Wave")
                    );

            loadedProgram.lfo2Wave =
                static_cast<int>(
                    programObject->getProperty("lfo2Wave")
                    );

            auto temperamentVar =
                programObject->getProperty("temperament");

            if (auto* temperamentArray = temperamentVar.getArray())
            {
                const int count =
                    juce::jmin(
                        12,
                        temperamentArray->size()
                    );

                for (int i = 0; i < count; ++i)
                {
                    loadedProgram.temperament[i] =
                        static_cast<int8_t>(
                            static_cast<int>(
                                (*temperamentArray)[i]
                                )
                            );
                }
            }


            DBG(
                "PROGRAM RESTORED NAME = "
                + juce::String(loadedProgram.name)
            );

            DBG(
                "PROGRAM RESTORED PAN = "
                + juce::String(loadedProgram.pan)
            );

            DBG(
                "RESTORED LFO1 RATE = "
                + juce::String(loadedProgram.lfo1Rate)
            );

            DBG(
                "RESTORED LFO2 DEPTH = "
                + juce::String(loadedProgram.lfo2Depth)
            );

            DBG(
                "RESTORED MOD WHEEL DEPTH = "
                + juce::String(loadedProgram.modWheelDepth)
            );

            DBG("BEFORE SETPROGRAM FROM PROJECT");

            auto keygroupsVar =
                root->getProperty("keygroups");

            auto* keygroupsArray =
                keygroupsVar.getArray();

            if (keygroupsArray == nullptr)
            {
                DBG("PROJECT LOAD FAILED: KEYGROUPS IS NOT ARRAY");
                return;
            }

            loadedProgram.keygroups.clear();

            for (const auto& keygroupVar : *keygroupsArray)
            {
                auto* keygroupObject =
                    keygroupVar.getDynamicObject();

                if (keygroupObject == nullptr)
                    continue;

                Keygroup kg;

                kg.id =
                    static_cast<uint8_t>(
                        static_cast<int>(
                            keygroupObject->getProperty("id")
                            )
                        );

                kg.nextAddress =
                    static_cast<uint16_t>(
                        static_cast<int>(
                            keygroupObject->getProperty("nextAddress")
                            )
                        );

                kg.filter.velocityToFreq =
                    static_cast<int>(
                        keygroupObject->getProperty("filterVelocityToFreq")
                        );

                kg.filter.pressureToFreq =
                    static_cast<int>(
                        keygroupObject->getProperty("filterPressureToFreq")
                        );

                kg.filter.envelopeToFreq =
                    static_cast<int>(
                        keygroupObject->getProperty("filterEnvelopeToFreq")
                        );


                kg.lowNote =
                    static_cast<int>(
                        keygroupObject->getProperty("lowNote")
                        );

                kg.highNote =
                    static_cast<int>(
                        keygroupObject->getProperty("highNote")
                        );

                kg.tune =
                    static_cast<int>(
                        keygroupObject->getProperty("tune")
                        );

                kg.filter.freq =
                    static_cast<int>(
                        keygroupObject->getProperty("filterFreq")
                        );

                kg.filter.keyFollow =
                    static_cast<int>(
                        keygroupObject->getProperty("filterKeyFollow")
                        );

                kg.filter.resonance =
                    static_cast<int>(
                        keygroupObject->getProperty("filterResonance")
                        );

                kg.lfo1Pitch =
                    static_cast<int>(
                        keygroupObject->getProperty("lfo1Pitch")
                        );

                kg.modVPitch =
                    static_cast<int>(
                        keygroupObject->getProperty("modVPitch")
                        );

                kg.modFilter1 =
                    static_cast<int>(
                        keygroupObject->getProperty("modFilter1")
                        );

                kg.modFilter2 =
                    static_cast<int>(
                        keygroupObject->getProperty("modFilter2")
                        );

                auto env1Var =
                    keygroupObject->getProperty("env1");

                if (auto* env1Object = env1Var.getDynamicObject())
                {
                    kg.env1.attack =
                        static_cast<int>(
                            env1Object->getProperty("attack")
                            );

                    kg.env1.decay =
                        static_cast<int>(
                            env1Object->getProperty("decay")
                            );

                    kg.env1.sustain =
                        static_cast<int>(
                            env1Object->getProperty("sustain")
                            );

                    kg.env1.release =
                        static_cast<int>(
                            env1Object->getProperty("release")
                            );
                }


                auto env2Var =
                    keygroupObject->getProperty("env2");

                if (auto* env2Object = env2Var.getDynamicObject())
                {
                    kg.env2.r1 =
                        static_cast<int>(
                            env2Object->getProperty("r1")
                            );

                    kg.env2.l1 =
                        static_cast<int>(
                            env2Object->getProperty("l1")
                            );

                    kg.env2.r2 =
                        static_cast<int>(
                            env2Object->getProperty("r2")
                            );

                    kg.env2.l2 =
                        static_cast<int>(
                            env2Object->getProperty("l2")
                            );

                    kg.env2.r3 =
                        static_cast<int>(
                            env2Object->getProperty("r3")
                            );

                    kg.env2.l3 =
                        static_cast<int>(
                            env2Object->getProperty("l3")
                            );

                    kg.env2.r4 =
                        static_cast<int>(
                            env2Object->getProperty("r4")
                            );

                    kg.env2.l4 =
                        static_cast<int>(
                            env2Object->getProperty("l4")
                            );

                    kg.env2.velAttack =
                        static_cast<int>(
                            env2Object->getProperty("velAttack")
                            );

                    kg.env2.velRelease =
                        static_cast<int>(
                            env2Object->getProperty("velRelease")
                            );

                    kg.env2.noteOffRelease =
                        static_cast<int>(
                            env2Object->getProperty("noteOffRelease")
                            );

                    kg.env2.keyTracking =
                        static_cast<int>(
                            env2Object->getProperty("keyTracking")
                            );
                }

                kg.modFilter3 =
                    static_cast<int>(
                        keygroupObject->getProperty("modFilter3")
                        );


                auto playbackVar =
                    keygroupObject->getProperty("playback");

                if (auto* playbackObject = playbackVar.getDynamicObject())
                {
                    kg.playback.kbeat =
                        static_cast<int>(
                            playbackObject->getProperty("kbeat")
                            );

                    kg.playback.ahold =
                        static_cast<uint8_t>(
                            static_cast<int>(
                                playbackObject->getProperty("ahold")
                                )
                            );

                    kg.playback.constantPitch =
                        static_cast<uint8_t>(
                            static_cast<int>(
                                playbackObject->getProperty("constantPitch")
                                )
                            );

                    kg.playback.keygroupXfade =
                        static_cast<int>(
                            playbackObject->getProperty("keygroupXfade")
                            );
                }


                auto velocityVar =
                    keygroupObject->getProperty("velocity");

                if (auto* velocityObject = velocityVar.getDynamicObject())
                {
                    kg.velocity.vAtt2 =
                        static_cast<int8_t>(
                            static_cast<int>(
                                velocityObject->getProperty("vAtt2")
                                )
                            );

                    kg.velocity.vRel2 =
                        static_cast<int8_t>(
                            static_cast<int>(
                                velocityObject->getProperty("vRel2")
                                )
                            );

                    kg.velocity.oRel2 =
                        static_cast<int8_t>(
                            static_cast<int>(
                                velocityObject->getProperty("oRel2")
                                )
                            );

                    kg.velocity.kDar2 =
                        static_cast<int8_t>(
                            static_cast<int>(
                                velocityObject->getProperty("kDar2")
                                )
                            );

                    kg.velocity.vEnv2 =
                        static_cast<int8_t>(
                            static_cast<int>(
                                velocityObject->getProperty("vEnv2")
                                )
                            );

                    kg.velocity.ePtch =
                        static_cast<uint8_t>(
                            static_cast<int>(
                                velocityObject->getProperty("ePtch")
                                )
                            );

                    kg.velocity.vxFade =
                        static_cast<uint8_t>(
                            static_cast<int>(
                                velocityObject->getProperty("vxFade")
                                )
                            );

                    kg.velocity.vZones =
                        static_cast<uint8_t>(
                            static_cast<int>(
                                velocityObject->getProperty("vZones")
                                )
                            );

                    kg.velocity.lkxf =
                        static_cast<uint8_t>(
                            static_cast<int>(
                                velocityObject->getProperty("lkxf")
                                )
                            );

                    kg.velocity.rkxf =
                        static_cast<uint8_t>(
                            static_cast<int>(
                                velocityObject->getProperty("rkxf")
                                )
                            );
                }

                auto zonesVar =
                    keygroupObject->getProperty("zones");

                auto* zonesArray =
                    zonesVar.getArray();

                if (zonesArray != nullptr)
                {
                    const int zoneCount =
                        juce::jmin(
                            4,
                            zonesArray->size()
                        );

                    for (int i = 0; i < zoneCount; ++i)
                    {
                        auto* zoneObject =
                            (*zonesArray)[i].getDynamicObject();

                        if (zoneObject == nullptr)
                            continue;

                        auto& zone = kg.zones[i];

                        zone.sampleName =
                            zoneObject->getProperty("sampleName")
                            .toString();

                        zone.sampleId =
                            static_cast<int>(
                                zoneObject->getProperty("sampleId")
                                );

                        zone.lowVel =
                            static_cast<uint8_t>(
                                static_cast<int>(
                                    zoneObject->getProperty("lowVel")
                                    )
                                );

                        zone.highVel =
                            static_cast<uint8_t>(
                                static_cast<int>(
                                    zoneObject->getProperty("highVel")
                                    )
                                );

                        zone.semitone =
                            static_cast<int>(
                                zoneObject->getProperty("semitone")
                                );

                        zone.fineTuneRaw =
                            static_cast<int>(
                                zoneObject->getProperty("fineTuneRaw")
                                );

                        zone.loudness =
                            static_cast<int8_t>(
                                static_cast<int>(
                                    zoneObject->getProperty("loudness")
                                    )
                                );

                        zone.filterFreq =
                            static_cast<int8_t>(
                                static_cast<int>(
                                    zoneObject->getProperty("filterFreq")
                                    )
                                );

                        zone.pan =
                            static_cast<int8_t>(
                                static_cast<int>(
                                    zoneObject->getProperty("pan")
                                    )
                                );

                        zone.playMode =
                            static_cast<PlayMode>(
                                static_cast<int>(
                                    zoneObject->getProperty("playMode")
                                    )
                                );

                        zone.lowVelXFade =
                            static_cast<uint8_t>(
                                static_cast<int>(
                                    zoneObject->getProperty("lowVelXFade")
                                    )
                                );

                        zone.highVelXFade =
                            static_cast<uint8_t>(
                                static_cast<int>(
                                    zoneObject->getProperty("highVelXFade")
                                    )
                                );

                        zone.constantPitch =
                            static_cast<bool>(
                                zoneObject->getProperty("constantPitch")
                                );
                    }
                }

                loadedProgram.keygroups.push_back(
                    std::move(kg)
                );
            }

            loadedProgram.groups =
                static_cast<int>(
                    loadedProgram.keygroups.size()
                    );

            DBG(
                "KEYGROUPS RESTORED = "
                + juce::String(loadedProgram.keygroups.size())
            );

            if (!loadedProgram.keygroups.empty())
            {
                DBG(
                    "KG1 RANGE = "
                    + juce::String(loadedProgram.keygroups[0].lowNote)
                    + " - "
                    + juce::String(loadedProgram.keygroups[0].highNote)
                );

                DBG(
                    "KG1 ZONE1 SAMPLE = "
                    + loadedProgram.keygroups[0].zones[0].sampleName
                );

                const auto& kg =
                    loadedProgram.keygroups[0];


                DBG(
                    "KG1 PLAYBACK KBEAT = "
                    + juce::String(kg.playback.kbeat)
                );

                DBG(
                    "KG1 VELOCITY VATT2 = "
                    + juce::String(static_cast<int>(kg.velocity.vAtt2))
                );

                DBG(
                    "KG1 VELOCITY EPTCH = "
                    + juce::String(static_cast<int>(kg.velocity.ePtch))
                );

                DBG(
                    "KG1 ENV1 ATTACK = "
                    + juce::String(kg.env1.attack)
                );

                DBG(
                    "KG1 ENV2 L1 = "
                    + juce::String(kg.env2.l1)
                );

                DBG(
                    "KG1 ENV2 R4 = "
                    + juce::String(kg.env2.r4)
                );
            }

            auto sampleHeadersVar =
                root->getProperty("sampleHeaders");

            if (auto* sampleHeadersArray = sampleHeadersVar.getArray())
            {
                sampleHeaders.clear();

                for (const auto& sampleHeaderVar : *sampleHeadersArray)
                {
                    auto* sampleHeaderObject =
                        sampleHeaderVar.getDynamicObject();

                    if (sampleHeaderObject == nullptr)
                        continue;

                    const int sampleId =
                        static_cast<int>(
                            sampleHeaderObject->getProperty("sampleId")
                            );

                    SampleHeader header;

                    header.id =
                        static_cast<int>(
                            sampleHeaderObject->getProperty("id")
                            );

                    header.bandwidth =
                        static_cast<int>(
                            sampleHeaderObject->getProperty("bandwidth")
                            );

                    header.originalPitch =
                        static_cast<int>(
                            sampleHeaderObject->getProperty("originalPitch")
                            );

                    header.name =
                        sampleHeaderObject->getProperty("name").toString();

                    header.sampleRateValid =
                        static_cast<int>(
                            sampleHeaderObject->getProperty("sampleRateValid")
                            );

                    header.numLoops =
                        static_cast<int>(
                            sampleHeaderObject->getProperty("numLoops")
                            );

                    header.activeLoop =
                        static_cast<int>(
                            sampleHeaderObject->getProperty("activeLoop")
                            );

                    header.highestLoop =
                        static_cast<int>(
                            sampleHeaderObject->getProperty("highestLoop")
                            );

                    header.playType =
                        static_cast<int>(
                            sampleHeaderObject->getProperty("playType")
                            );

                    header.tune =
                        static_cast<double>(
                            sampleHeaderObject->getProperty("tune")
                            );

                    header.location =
                        static_cast<uint32_t>(
                            static_cast<juce::int64>(
                                sampleHeaderObject->getProperty("location")
                                )
                            );

                    header.length =
                        static_cast<uint64_t>(
                            static_cast<juce::int64>(
                                sampleHeaderObject->getProperty("length")
                                )
                            );

                    header.start =
                        static_cast<uint32_t>(
                            static_cast<juce::int64>(
                                sampleHeaderObject->getProperty("start")
                                )
                            );

                    header.end =
                        static_cast<uint32_t>(
                            static_cast<juce::int64>(
                                sampleHeaderObject->getProperty("end")
                                )
                            );

                    header.spare =
                        static_cast<uint8_t>(
                            static_cast<int>(
                                sampleHeaderObject->getProperty("spare")
                                )
                            );

                    header.waveComment =
                        static_cast<uint8_t>(
                            static_cast<int>(
                                sampleHeaderObject->getProperty("waveComment")
                                )
                            );

                    header.stereoPartner =
                        static_cast<uint16_t>(
                            static_cast<int>(
                                sampleHeaderObject->getProperty("stereoPartner")
                                )
                            );

                    header.sampleRate =
                        static_cast<uint16_t>(
                            static_cast<int>(
                                sampleHeaderObject->getProperty("sampleRate")
                                )
                            );

                    header.holdLoopTune =
                        static_cast<int8_t>(
                            static_cast<int>(
                                sampleHeaderObject->getProperty("holdLoopTune")
                                )
                            );

                    auto loopsVar =
                        sampleHeaderObject->getProperty("loops");

                    if (auto* loopsArray = loopsVar.getArray())
                    {
                        const int loopCount =
                            juce::jmin(
                                4,
                                loopsArray->size()
                            );

                        for (int i = 0; i < loopCount; ++i)
                        {
                            auto* loopObject =
                                (*loopsArray)[i].getDynamicObject();

                            if (loopObject == nullptr)
                                continue;

                            auto& loop = header.loops[i];

                            loop.position =
                                static_cast<uint32_t>(
                                    static_cast<juce::int64>(
                                        loopObject->getProperty("position")
                                        )
                                    );

                            loop.length =
                                static_cast<double>(
                                    loopObject->getProperty("length")
                                    );

                            loop.dwell =
                                static_cast<uint16_t>(
                                    static_cast<int>(
                                        loopObject->getProperty("dwell")
                                        )
                                    );

                            loop.relativeFactors =
                                static_cast<uint32_t>(
                                    static_cast<juce::int64>(
                                        loopObject->getProperty("relativeFactors")
                                        )
                                    );
                        }
                    }

                    sampleHeaders[sampleId] = std::move(header);
                }
            }

            DBG(
                "SAMPLE HEADERS RESTORED = "
                + juce::String(sampleHeaders.size())
            );

            programEditor.setProgram(loadedProgram);

            DBG("AFTER SETPROGRAM FROM PROJECT");

            keygroupMap.setProgram(loadedProgram);

            programTree.setProgram(
                loadedProgram,
                sampleHeaders
            );

            DBG("PROJECT TREE UPDATED");
        }
    );
}

bool MainComponent::connectS3000Midi(
    int inputIndex,
    int outputIndex,
    bool saveAfterConfirmation)
{
    s3000MidiInputDevices =
        juce::MidiInput::getAvailableDevices();

    s3000MidiOutputDevices =
        juce::MidiOutput::getAvailableDevices();

    if (inputIndex < 0 ||
        inputIndex >= s3000MidiInputDevices.size())
    {
        DBG("Invalid S3000 MIDI input index");
        return false;
    }

    if (outputIndex < 0 ||
        outputIndex >= s3000MidiOutputDevices.size())
    {
        DBG("Invalid S3000 MIDI output index");
        return false;
    }

    DBG("OPEN S3000 INPUT: "
        + s3000MidiInputDevices[inputIndex].name);

    DBG("OPEN S3000 OUTPUT: "
        + s3000MidiOutputDevices[outputIndex].name);

    if (!midiManager.openInput(
        inputIndex,
        this))
    {
        DBG("S3000 MIDI INPUT OPEN FAILED");
        return false;
    }

    if (!midiManager.openOutput(outputIndex))
    {
        DBG("S3000 MIDI OUTPUT OPEN FAILED");
        return false;
    }

    sysExSender.setMidiOutput(
        midiManager.getOutput()
    );

    pendingMidiDeviceSave =
        saveAfterConfirmation;

    if (saveAfterConfirmation)
    {
        pendingMidiInputDevice =
            s3000MidiInputDevices[inputIndex];

        pendingMidiOutputDevice =
            s3000MidiOutputDevices[outputIndex];
    }

    waitingForInitialMidiResponse = true;

    midiConnectionAttemptTime =
        juce::Time::getMillisecondCounterHiRes();

    DBG("MIDI PORTS OPEN");
    DBG("WAITING FOR S3000XL RESPONSE");

    juce::Timer::callAfterDelay(
        500,
        [this]
        {
            if (midiManager.getOutput() != nullptr)
            {
                DBG("AUTO REQUEST PROGRAM LIST");
                sysExSender.sendRPLIST();
            }
        }
    );

    return true;
}

void MainComponent::saveS3000MidiDevices(
    const juce::MidiDeviceInfo& input,
    const juce::MidiDeviceInfo& output)
{
    if (midiProperties == nullptr)
        return;

    midiProperties->setValue(
        "s3000MidiInputId",
        input.identifier
    );

    midiProperties->setValue(
        "s3000MidiOutputId",
        output.identifier
    );

    midiProperties->saveIfNeeded();

    DBG("SAVED MIDI INPUT: "
        + input.name
        + " / "
        + input.identifier);

    DBG("SAVED MIDI OUTPUT: "
        + output.name
        + " / "
        + output.identifier);
}

bool MainComponent::restoreS3000MidiDevices()
{
    if (midiProperties == nullptr)
        return false;

    const auto savedInputId =
        midiProperties->getValue(
            "s3000MidiInputId"
        );

    const auto savedOutputId =
        midiProperties->getValue(
            "s3000MidiOutputId"
        );

    if (savedInputId.isEmpty() ||
        savedOutputId.isEmpty())
    {
        DBG("NO SAVED S3000 MIDI DEVICES");
        return false;
    }

    s3000MidiInputDevices =
        juce::MidiInput::getAvailableDevices();

    s3000MidiOutputDevices =
        juce::MidiOutput::getAvailableDevices();

    int inputIndex = -1;
    int outputIndex = -1;

    for (int i = 0;
        i < s3000MidiInputDevices.size();
        ++i)
    {
        DBG("INPUT "
            + juce::String(i)
            + " NAME="
            + s3000MidiInputDevices[i].name
            + " ID="
            + s3000MidiInputDevices[i].identifier);

        if (s3000MidiInputDevices[i].identifier ==
            savedInputId)
        {
            inputIndex = i;
        }
    }

    for (int i = 0;
        i < s3000MidiOutputDevices.size();
        ++i)
    {
        DBG("OUTPUT "
            + juce::String(i)
            + " NAME="
            + s3000MidiOutputDevices[i].name
            + " ID="
            + s3000MidiOutputDevices[i].identifier);

        if (s3000MidiOutputDevices[i].identifier ==
            savedOutputId)
        {
            outputIndex = i;
        }
    }

    if (inputIndex < 0 || outputIndex < 0)
    {
        DBG("SAVED S3000 MIDI DEVICE NOT FOUND");
        return false;
    }

    DBG("RESTORING SAVED S3000 MIDI CONNECTION");

    return connectS3000Midi(
        inputIndex,
        outputIndex,
        false
    );
}

void MainComponent::showS3000MidiSetup()
{
    s3000MidiInputDevices =
        juce::MidiInput::getAvailableDevices();

    s3000MidiOutputDevices =
        juce::MidiOutput::getAvailableDevices();

    if (s3000MidiInputDevices.isEmpty() ||
        s3000MidiOutputDevices.isEmpty())
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::MessageBoxIconType::WarningIcon,
            "MIDI Setup",
            "No MIDI input/output devices were found."
        );

        return;
    }

    auto* window =
        new juce::AlertWindow(
            "S3000XL MIDI Setup",
            "Select the MIDI interface connected to your S3000XL.",
            juce::MessageBoxIconType::NoIcon
        );

    // ==============================
    // MIDI Input
    // ==============================

    juce::StringArray inputNames;

    for (const auto& device : s3000MidiInputDevices)
        inputNames.add(device.name);

    window->addComboBox(
        "midiInput",
        inputNames,
        "MIDI Input"
    );

    auto* inputCombo =
        window->getComboBoxComponent(
            "midiInput"
        );

    // ==============================
    // MIDI Output
    // ==============================

    juce::StringArray outputNames;

    for (const auto& device : s3000MidiOutputDevices)
        outputNames.add(device.name);

    window->addComboBox(
        "midiOutput",
        outputNames,
        "MIDI Output"
    );

    auto* outputCombo =
        window->getComboBoxComponent(
            "midiOutput"
        );

    // ==============================
    // Buttons
    // ==============================

    window->addButton(
        "Connect",
        1,
        juce::KeyPress(
            juce::KeyPress::returnKey
        )
    );

    window->addButton(
        "Cancel",
        0,
        juce::KeyPress(
            juce::KeyPress::escapeKey
        )
    );

    // ==============================
    // Show dialog
    // ==============================

    window->enterModalState(
        true,
        juce::ModalCallbackFunction::create(
            [this, window, inputCombo, outputCombo]
            (int result)
            {
                if (result == 1)
                {
                    const int inputIndex =
                        inputCombo->getSelectedItemIndex();

                    const int outputIndex =
                        outputCombo->getSelectedItemIndex();

                    if (inputIndex < 0 ||
                        outputIndex < 0)
                    {
                        juce::AlertWindow::
                            showMessageBoxAsync(
                                juce::MessageBoxIconType::WarningIcon,
                                "MIDI Setup",
                                "Please select both a MIDI input and output."
                            );
                    }
                    else
                    {
                        if (!connectS3000Midi(
                            inputIndex,
                            outputIndex,
                            true))
                        {
                            juce::AlertWindow::
                                showMessageBoxAsync(
                                    juce::MessageBoxIconType::WarningIcon,
                                    "MIDI Setup",
                                    "Could not open the selected MIDI devices."
                                );
                        }
                    }
                }

                delete window;
            }
        ),
        false
    );
}


// write-test
