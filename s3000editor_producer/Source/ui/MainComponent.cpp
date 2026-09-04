#include "MainComponent.h"
#include <fstream>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../s3000/Offsets.h"
#include "../s3000/ProgramParser.h"
#include "../s3000/KeygroupParser.h"
#include "../s3000/SampleHeaderParser.h"
#include "../s3000/KeygroupHeaderParser.h"
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

std::optional<int> findPanOffset(const std::vector<uint8_t>& a,
    const std::vector<uint8_t>& b,
    const std::vector<uint8_t>& c)
{
    size_t size = juce::jmin( a.size(),
        juce::jmin(b.size(),c.size()) );

    int bestIndex = -1;
    int bestScore = 0;

    for (size_t i = 0; i < size; ++i)
    {
        int da = (int)a[i];
        int db = (int)b[i];
        int dc = (int)c[i];


        int score = std::abs(da - db) + std::abs(dc - db);

        if (da == db && db == dc)
        {
            continue;
        }

        if (score > bestScore)
        {
            bestScore = score;
            bestIndex = (int)i;
        }
    }

    if (bestScore < 5)
    {
        return std::nullopt;
    }

    return bestIndex;

}

//==============================================================================
MainComponent::MainComponent()
{
    setSize(900, 800);

    addAndMakeVisible(listBox);
    listBox.setModel(this);

    addAndMakeVisible(programTree);
    //addAndMakeVisible(sampleHeaderEditor);
    //
    //addAndMakeVisible(sampleHeaderViewport);

    addAndMakeVisible(editorTabs);

    addAndMakeVisible(auditionButton);
    auditionButton.setAlwaysOnTop(true);

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

    //addAndMakeVisible(keygroupMap);

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

            DBG(
                "MAP SELECT KG MODVPITCH KG="
                + juce::String(currentKeygroup)
                + " VALUE="
                + juce::String(modVPitch)
            );

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

            DBG(
                "MAP SELECT KG LFO1PITCH KG="
                + juce::String(currentKeygroup)
                + " VALUE="
                + juce::String(lfo1Pitch)
            );

            programEditor.setLfo1PitchAmount(
                lfo1Pitch
            );


            const auto& kg =
                loadedProgram.keygroups[index];

            DBG(
                "SELECTED KG "
                + juce::String(index)
                + " ZONE0 SAMPLE=["
                + kg.zones[0].sampleName
                + "] LOW="
                + juce::String(kg.zones[0].lowVel)
                + " HIGH="
                + juce::String(kg.zones[0].highVel)
            );

            keyGroupEditor.setKeygroup(
                kg,
                index
            );

            // ========================================
            // 4 Zone�S����Overview��
            // ========================================

            velocityZoneEditor.setZones(
                kg.zones
            );

            // ========================================
            // Zone 1���ڍ�Editor��
            // ========================================

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

            DBG(
                "KEYGROUP MAP SELECTED INDEX="
                + juce::String(index)
            );
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
        "Sample Header",
        juce::Colours::darkgrey,
        &sampleHeaderViewport,
        false
    );

    
    programEditor.onProgramChanged =
        [this](
            const Program& program)
        {

            DBG(
                "PROGRAM CHANGE MODSPITCH="
                + juce::String(program.modSPitch)
                + " MODVPITCH="
                + juce::String(program.modVPitch)
            );

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
                        DBG(
                            "PROGRAM RENAME REJECTED: DUPLICATE NAME = "
                            + newName
                        );

                        programEditor.setProgram(
                            loadedProgram
                        );

                        return;
                    }
                }
            }
            
            auto encodedProgram =
                ProgramEncoder::encode(
                    program
                );

            static std::vector<uint8_t> previousSentProgram;

            if (!previousSentProgram.empty()
                && previousSentProgram.size() == encodedProgram.size())
            {
                DBG("=== PDATA DIFF BEFORE SEND ===");

                for (size_t i = 0; i < encodedProgram.size(); ++i)
                {
                    if (encodedProgram[i] != previousSentProgram[i])
                    {
                        DBG(
                            "OFFSET "
                            + juce::String((int)i)
                            + ": "
                            + juce::String((int)previousSentProgram[i])
                            + " -> "
                            + juce::String((int)encodedProgram[i])
                        );
                    }
                }
            }

            previousSentProgram = encodedProgram;


            DBG(
                "MAIN MODSPITCH="
                + juce::String(program.modSPitch)
            );

            DBG(
                "ENCODED MODSPITCH OFFSET="
                + juce::String((int)ProgramOffset::Mod::ModSPitch)
                + " RAW="
                + juce::String(
                    (int)encodedProgram[
                        ProgramOffset::Mod::ModSPitch
                    ]
                )
            );



            DBG(
                "MAIN MODVPITCH="
                + juce::String(program.modVPitch)
            );

            DBG(
                "ENCODED MODVPITCH OFFSET="
                + juce::String((int)ProgramOffset::Mod::ModVPitch)
                + " RAW="
                + juce::String(
                    (int)encodedProgram[
                        ProgramOffset::Mod::ModVPitch
                    ]
                )
            );


            if (encodedProgram.empty())
            {
                DBG(
                    "PROGRAM EDITOR ENCODE FAILED"
                );

                return;
            }

            DBG(
                "OUTPUT RAW = "
                + juce::String(
                    program.output
                )
            );

            DBG(
                "=== PROGRAM EDITOR WRITE ==="
            );

            DBG(
                "PLAY LOW = "
                + juce::String(
                    program.playLow
                )
            );

            DBG(
                "PLAY HIGH = "
                + juce::String(
                    program.playHigh
                )
            );

            DBG(
                "PAN = "
                + juce::String(program.pan)
            );

            DBG(
                "OUT LEVEL = "
                + juce::String(
                    program.individualOutputLevel
                )
            );

            DBG("PROGRAM WRITE: EXPECT REFRESH-ONLY RESPONSE");

            programRefreshOnly = true;

            sysExSender.sendProgramData(
                program.programNumber,
                encodedProgram
            );

            // 診断用
            sysExSender.sendProgramHeader(
                program.programNumber
            );

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
                DBG(
                    "PROGRAM MOD PITCH SOURCE WRITE VALUE="
                    + juce::String(value)
                );

                DBG("SOURCE STEP 1");

                sysExSender.sendProgramHeaderByte(
                    loadedProgram.programNumber,
                    ProgramOffset::Mod::ModSPitch,
                    value
                );

                DBG("SOURCE STEP 2");

                loadedProgram.modSPitch = value;

                DBG("SOURCE STEP 3");

                

            };

        programEditor.onModFilter1Changed =
            [this](int value)
            {
                if (currentKeygroup < 0 ||
                    currentKeygroup >=
                    static_cast<int>(
                        loadedProgram.keygroups.size()
                        ))
                {
                    DBG("FILTER1 AMOUNT: INVALID KEYGROUP");
                    return;
                }

                auto& keygroup =
                    loadedProgram.keygroups[currentKeygroup];

                keygroup.modFilter1 = value;

                DBG(
                    "KG FILTER1 AMOUNT WRITE KG="
                    + juce::String(currentKeygroup)
                    + " VALUE="
                    + juce::String(value)
                );

                sysExSender.sendKeygroupHeaderByte(
                    loadedProgram.programNumber,
                    currentKeygroup,
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
                        loadedProgram.keygroups.size()
                        ))
                {
                    DBG("FILTER2 AMOUNT: INVALID KEYGROUP");
                    return;
                }

                auto& keygroup =
                    loadedProgram.keygroups[currentKeygroup];

                keygroup.modFilter2 = value;

                DBG(
                    "KG FILTER2 AMOUNT WRITE KG="
                    + juce::String(currentKeygroup)
                    + " VALUE="
                    + juce::String(value)
                );

                sysExSender.sendKeygroupHeaderByte(
                    loadedProgram.programNumber,
                    currentKeygroup,
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
                        loadedProgram.keygroups.size()
                        ))
                {
                    DBG("FILTER3 AMOUNT: INVALID KEYGROUP");
                    return;
                }

                auto& keygroup =
                    loadedProgram.keygroups[currentKeygroup];

                keygroup.modFilter3 = value;

                DBG(
                    "KG FILTER3 AMOUNT WRITE KG="
                    + juce::String(currentKeygroup)
                    + " VALUE="
                    + juce::String(value)
                );

                sysExSender.sendKeygroupHeaderByte(
                    loadedProgram.programNumber,
                    currentKeygroup,
                    KeygroupHeaderOffset::Mod::Filter3,
                    value
                );
            };

        programEditor.onEnv2PitchChanged =
            [this](int value)
            {
                if (currentKeygroup < 0 ||
                    currentKeygroup >=
                    static_cast<int>(loadedProgram.keygroups.size()))
                {
                    DBG("ENV2 PITCH: INVALID KEYGROUP");
                    return;
                }

                auto& keygroup =
                    loadedProgram.keygroups[currentKeygroup];

                keygroup.modVPitch = value;

                DBG(
                    "KG ENV2 PITCH WRITE KG="
                    + juce::String(currentKeygroup)
                    + " VALUE="
                    + juce::String(keygroup.modVPitch)
                );

                sysExSender.sendKeygroupHeaderByte(
                    loadedProgram.programNumber,
                    currentKeygroup,
                    KeygroupHeaderOffset::Mod::ModVPitch,
                    keygroup.modVPitch
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

                DBG(
                    "KG LFO1 PITCH WRITE KG="
                    + juce::String(currentKeygroup)
                    + " VALUE="
                    + juce::String(keygroup.lfo1Pitch)
                );

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

            DBG(
                "CURRENT ZONE = "
                + juce::String(currentZone)
            );
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

                DBG(
                    "SELECT KG MODVPITCH KG="
                    + juce::String(currentKeygroup)
                    + " VALUE="
                    + juce::String(modVPitch)
                );

                programEditor.setModPitchAmount(
                    modVPitch
                );
            }

            DBG("MAIN COMPONENT RECEIVED ZONE SELECTION");

            DBG(
                "KEYGROUP INDEX = "
                + juce::String(currentKeygroup)
            );

            DBG(
                "ZONE INDEX = "
                + juce::String(currentZone)
            );

            // ========================================
            // 4 Zone�S����Overview��
            // ========================================

            if (currentKeygroup >= 0 &&
                currentKeygroup <
                static_cast<int>(
                    loadedProgram.keygroups.size()
                    ))
            {
                DBG("ABOUT TO CALL setZones");

                velocityZoneEditor.setZones(
                    loadedProgram
                    .keygroups[currentKeygroup]
                    .zones
                );

                DBG("AFTER CALL setZones");
            }

            // ========================================
            // �I��Zone���ڍ�Editor��
            // ========================================

            DBG(
                "TREE SELECT ZONE SAMPLE=["
                + zone.sampleName
                + "] ID="
                + juce::String(zone.sampleId)
            );

            DBG(
                "TREE SELECT HEADER NAME=["
                + sampleHeader.name
                + "] ID="
                + juce::String(sampleHeader.id)
            );


            velocityZoneEditor.setZone(
                zone
            );

            sampleHeaderEditor.setSampleHeader(
                sampleHeader
            );

            editorTabs.setCurrentTabIndex(1);
        };

    addAndMakeVisible(programCombo);

    deviceStatusLabel.setText(
        "S3000XL: Waiting for device...",
        juce::dontSendNotification
    );

    deviceStatusLabel.setColour(
        juce::Label::textColourId,
        juce::Colours::white
    );

    // �f�o�b�O�p�F�\���̈���m�F
    //deviceStatusLabel.setColour(
    //    juce::Label::backgroundColourId,
    //    juce::Colours::darkgrey
    //);

    addAndMakeVisible(deviceStatusLabel);


    velocityZoneEditor.onZoneRangeChanged =
        [this](
            int zoneIndex,
            int low,
            int high
            )
        {
            auto& kg =
                loadedProgram.keygroups[
                    currentKeygroup
                ];

            auto& zone =
                kg.zones[zoneIndex];

            zone.lowVel =
                static_cast<uint8_t>(low);

            zone.highVel =
                static_cast<uint8_t>(high);

            currentZone =
                zoneIndex;

            velocityZoneEditor.setZone(
                zone
            );

            auto encoded =
                KeygroupEncoder::encode(
                    kg
                );

            if (!encoded.empty())
            {
                sysExSender.sendKeygroupData(
                    loadedProgram.programNumber,
                    currentKeygroup,
                    encoded
                );


            }
        };

    keygroupTitleLabel.setText(
        "KEYGROUPS",
        juce::dontSendNotification
    );

    keygroupTitleLabel.setColour(
        juce::Label::textColourId,
        juce::Colours::lightgrey
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
            DBG(
                "MAIN COMPONENT RECEIVED BASIC ZONE SELECTION"
            );

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
                    DBG(
                        "BASIC ZONE FOUND SAMPLE HEADER ID="
                        + juce::String(zone.sampleId)
                    );

                    sampleHeaderEditor.setSampleHeader(
                        it->second
                    );
                }
                else
                {
                    DBG(
                        "BASIC ZONE SAMPLE HEADER NOT LOADED -> REQUEST ID="
                        + juce::String(zone.sampleId)
                    );

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

            DBG("=== PROGRAM WRITE TEST ===");

            DBG(
                "PAN = "
                + juce::String(testProgram.pan)
            );

            DBG(
                "LOUDNESS = "
                + juce::String(
                    testProgram.loudness
                )
            );

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
            DBG(
                "MAIN COMPONENT RECEIVED KEYGROUP SELECTION "
                + juce::String(keygroupIndex)
            );

            currentKeygroup = keygroupIndex;

            // Keygroup��I��������Zone 1��I����Ԃɂ���
            currentZone = 0;

            editorTabs.setCurrentTabIndex(0);

            keyGroupEditor.setKeygroup(
                keygroup,
                keygroupIndex
            );

            // ========================================
            // 4 Zone Overview�X�V
            // ========================================

            velocityZoneEditor.setZones(
                keygroup.zones
            );

            velocityZoneEditor.setSelectedZone(
                currentZone
            );

            // ========================================
            // Zone 1���ڍ�Editor�ɂ��Z�b�g
            // ========================================

            velocityZoneEditor.setZone(
                keygroup.zones[currentZone]
            );
        };

    //addAndMakeVisible(keyGroupEditor);

    //addAndMakeVisible(keyGroupViewport);




    //programTree.setProgram(currentProgramData, sampleHeaders);

    //addAndMakeVisible(velocityZoneEditor);
   /* addAndMakeVisible(captureAButton);

    captureAButton.onClick = [this]
        {
            saveDump("dump_A");
            DBG("Captured A");
        };

    addAndMakeVisible(captureBButton);
    captureBButton.onClick = [this]
    {
        saveDump("dump_B");
        DBG("Captured B");
    };

    addAndMakeVisible(compareButton);
    compareButton.onClick = [this]
        {
            compareLatest();
        };*/

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

            DBG("=== PROGRAM MIDI CHANNEL WRITE TEST ===");

            DBG(
                "MIDI CHANNEL RAW = "
                + juce::String(testProgram.midiChannel)
            );


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

            DBG(
                "===== TEST DELETE KEYGROUP ====="
            );

            DBG(
                "DELETE KG INDEX="
                + juce::String(deleteIndex)
            );

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


    #if JUCE_DEBUG




    addAndMakeVisible(requestButton);
    requestButton.onClick = [this]
        {
            sysExSender.sendRPLIST();
        };

    DBG("SETTING requestRPDATAButton CALLBACK");

    addAndMakeVisible(requestRPDATAButton);

    requestRPDATAButton.onStateChange = [this]()
        {
            DBG("GET PROGRAM STATE CHANGED");
            DBG(
                "DOWN = "
                + juce::String(
                    (int)requestRPDATAButton.isDown()
                )
            );
        };

    requestRPDATAButton.setTriggeredOnMouseDown(true);
#endif

//    requestRPDATAButton.onClick = [this]()
//        {
//            DBG("========== GET PROGRAM BUTTON CLICKED ==========");
//            DBG("REQUEST PROGRAM HEADER");
//
////            sendProgramHeader(0);
//            //sysExSender.sendProgramHeader(0);
//
//            if (currentProgram < 0)
//            {
//                DBG("GET PROGRAM: NO PROGRAM SELECTED");
//                return;
//            }
//
//            DBG(
//                "REQUEST PROGRAM HEADER INDEX="
//                + juce::String(currentProgram)
//            );
//
//            sysExSender.sendProgramHeader(
//                currentProgram
//            );
//
//        };

    DBG(
        "HAS ONCLICK = "
        + juce::String(
            requestRPDATAButton.onClick ? "YES" : "NO"
        )
    );

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

    DBG("requestRPDATAButton CALLBACK SET");



    //addAndMakeVisible(programLabel);
    //programLabel.setText("No program", juce::dontSendNotification);

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

            DBG(
                "KEYGROUP RANGE CHANGED KG="
                + juce::String(keygroupIndex)
                + " LOW="
                + juce::String(lowNote)
                + " HIGH="
                + juce::String(highNote)
            );

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

            DBG("===== MAP SEND =====");
            DBG("KG = " + juce::String(keygroupIndex));
            DBG("LOW = " + juce::String(lowNote));
            DBG("HIGH = " + juce::String(highNote));

            auto encoded =
                KeygroupEncoder::encode(kg);

            DBG(
                "ENCODE LOW="
                + juce::String(
                    (int)encoded[
                        KeygroupHeaderOffset::Common::LONOTE
                    ]
                )
            );

            DBG(
                "ENCODE HIGH="
                + juce::String(
                    (int)encoded[
                        KeygroupHeaderOffset::Common::HINOTE
                    ]
                )
            );

            sysExSender.sendKeygroupData(
                loadedProgram.programNumber,
                keygroupIndex,
                encoded
            );

            DBG(
                "KEYGROUP DATA SENT KG="
                + juce::String(keygroupIndex)
            );
        };


    //keygroupMap.onDeleteKeygroup =
    //    [this](int keygroupIndex)
    //    {
    //        if (loadedProgram.groups <= 1)
    //            return;
    //        pendingDeleteKeygroup =
    //            keygroupIndex;

    //        waitingForDeleteReply = true;

    //        sysExSender.sendDeleteKeygroup(
    //            loadedProgram.programNumber,
    //            keygroupIndex
    //        );
    //    };

    //keygroupMap.onAddKeygroup =
    //    [this]()
    //    {
    //        DBG("ADD KG UI CLICK");

    //        addKeygroup();
    //    };



    keyGroupEditor.onKeygroupChanged =
        [this](
            int keygroupIndex,
            const Keygroup& keygroup
            )
        {
            DBG("KEYGROUP CALLBACK FIRED");

            DBG("===== KEYGROUP UPDATED =====");
            DBG(
                "KEYGROUP = "
                + juce::String(keygroupIndex)
            );
            DBG(
                "LOW NOTE = "
                + juce::String(keygroup.lowNote)
            );
            DBG(
                "HIGH NOTE = "
                + juce::String(keygroup.highNote)
            );

            DBG(
                "loadedProgram.keygroups.size() = "
                + juce::String((int)loadedProgram.keygroups.size())
            );

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

            DBG("LOADED PROGRAM KEYGROUP UPDATED");

            //auto& updatedKeygroup =
            //    loadedProgram.keygroups[keygroupIndex];

            //auto encoded =
            //    KeygroupEncoder::encode(
            //        updatedKeygroup
            //    );

            if (encoded.empty())
            {
                DBG("KEYGROUP ENCODE FAILED");
                return;
            }

            DBG(
                "E_PTCH OFFSET=29 RAW="
                + juce::String(
                    (int)encoded[
                        KeygroupHeaderOffset::Velocity::E_PTCH
                    ]
                )
            );

            DBG(
                "ENCODED FILFRQ = "
                + juce::String(
                    encoded[
                        KeygroupHeaderOffset::Filter::FILFRQ
                    ]
                )
            );

            DBG(
                "ENCODED FILQ = "
                + juce::String(
                    encoded[
                        KeygroupHeaderOffset::Filter::FILQ
                    ]
                )
            );


            // �����m�F
            const auto& raw =
                updatedKeygroup.rawData;

            for (int i = 0; i < (int)encoded.size(); ++i)
            {
                if (raw[i] != encoded[i])
                {
                    DBG(
                        "ENCODE DIFF offset="
                        + juce::String(i)
                        + " "
                        + juce::String((int)raw[i])
                        + " -> "
                        + juce::String((int)encoded[i])
                    );
                }
            }


            sysExSender.sendKeygroupData(
                currentProgramIndex,
                keygroupIndex,
                encoded
            );

            sysExSender.sendKeygroupByte(
                currentProgramIndex,
                keygroupIndex,
                KeygroupHeaderOffset::Filter::FILQ,
                static_cast<uint8_t>(
                    storedKeygroup.filter.resonance
                    )
            );

        };

        /*rogramEditor.onEnv2PitchChanged =
            [this](int value)
            {
                if (currentKeygroup < 0 ||
                    currentKeygroup >=
                    static_cast<int>(loadedProgram.keygroups.size()))
                {
                    DBG("NO VALID KEYGROUP SELECTED");
                    return;
                }

                auto& keygroup =
                    loadedProgram.keygroups[currentKeygroup];

                keygroup.velocity.ePtch = value;

                DBG(
                    "PROGRAM TAB -> KEYGROUP E_PTCH = "
                    + juce::String(value)
                );

                auto encoded =
                    KeygroupEncoder::encode(keygroup);

                if (encoded.empty())
                {
                    DBG("KEYGROUP ENCODE FAILED");
                    return;
                }

                DBG(
                    "E_PTCH OFFSET=29 RAW="
                    + juce::String(
                        (int)encoded[
                            KeygroupHeaderOffset::Velocity::E_PTCH
                        ]
                    )
                );

                sysExSender.sendKeygroupData(
                    loadedProgram.programNumber,
                    currentKeygroup,
                    encoded
                );
            };*/



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

            const int constantPitchOffset =
                132 + currentZone;

            const uint8_t constantPitchValue =
                kg.zones[currentZone].constantPitch
                ? 1
                : 0;

            sysExSender.sendKeygroupByte(
                currentProgramIndex,
                currentKeygroup,
                constantPitchOffset,
                constantPitchValue
            );



            // ========================================
// Update Sample Header Editor
// ========================================

            if (zone.sampleId >= 0)
            {
                auto it =
                    sampleHeaders.find(zone.sampleId);

                if (it != sampleHeaders.end())
                {
                    sampleHeaderEditor.setSampleHeader(
                        it->second
                    );
                }
                else
                {
                    DBG(
                        "REQUEST SAMPLE HEADER ID="
                        + juce::String(zone.sampleId)
                    );

                    pendingSampleRequests.insert(
                        zone.sampleId
                    );

                    sysExSender.sendSampleHeader(
                        zone.sampleId
                    );
                }
            }

        };;














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

    midiManager.openInput(
        1,
        this
    );

    if (midiManager.openOutput(2))
    {
        sysExSender.setMidiOutput(
            midiManager.getOutput()
        );
    }

    juce::Timer::callAfterDelay(
        500,
        [this]
        {
            DBG("AUTO REQUEST PROGRAM LIST");
            sysExSender.sendRPLIST();
        }
    );


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

            //keyboardOctaveLabel.setVisible(showKeyboard);
            //keyboardOctaveCombo.setVisible(showKeyboard);

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

    //addAndMakeVisible(keyboardOctaveCombo);

    //keyboardOctaveCombo.addItem("C1", 1);
    //keyboardOctaveCombo.addItem("C2", 2);
    //keyboardOctaveCombo.addItem("C3", 3);
    //keyboardOctaveCombo.addItem("C4", 4);
    //keyboardOctaveCombo.addItem("C5", 5);

    //keyboardOctaveCombo.setSelectedId(
    //    3,
    //    juce::dontSendNotification
    //);

    //keyboardOctaveCombo.onChange = [this]()
    //    {
    //        const int octave =
    //            keyboardOctaveCombo.getSelectedId();

    //        const int noteNumber =
    //            octave * 12;

    //        keyboardComponent.setLowestVisibleKey(
    //            noteNumber
    //        );
    //    };

    addAndMakeVisible(deleteProgramButton);
    addAndMakeVisible(addProgramButton);


    deleteProgramButton.onClick =
        [this]()
        {
            const int programNumber =
                loadedProgram.programNumber;

            juce::AlertWindow::showOkCancelBox(
                juce::AlertWindow::WarningIcon,
                "Delete Program?",
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
        };


    addProgramButton.onClick =
        [this]()
        {
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

            sysExSender.sendProgramData(
                newProgramNumber,
                encodedProgram
            );
        };



    deviceStatusLabel.toFront(false);
    startTimer(1000);

}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::FontOptions (16.0f));
    g.setColour (juce::Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);
}



void MainComponent::resized()
{
    auto area = getLocalBounds();

    // =========================
    // Top bar
    // =========================
    auto topBar = area.removeFromTop(60);

    auto programArea =
        topBar.removeFromLeft(330)
        .reduced(20, 15);

    deleteProgramButton.setBounds(
        programArea.removeFromRight(30)
    );

    programArea.removeFromRight(6);

    addProgramButton.setBounds(
        programArea.removeFromRight(30)
    );

    programArea.removeFromRight(6);

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

        //keyboardOctaveCombo.setBounds(
        //    keyboardControls.removeFromLeft(80)
       /* );*/

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
        //keyboardOctaveCombo.setBounds({});
    }


#if JUCE_DEBUG
    // =========================
    // Debug toolbar
    // =========================
    auto debugRow = area.removeFromTop(32);
    debugRow = debugRow.reduced(4, 2);

    const int gap = 4;

    captureAButton.setBounds(
        debugRow.removeFromLeft(90)
    );

    debugRow.removeFromLeft(gap);

    captureBButton.setBounds(
        debugRow.removeFromLeft(90)
    );

    debugRow.removeFromLeft(gap);

    compareButton.setBounds(
        debugRow.removeFromLeft(90)
    );

    debugRow.removeFromLeft(gap);

    requestButton.setBounds(
        debugRow.removeFromLeft(110)
    );

    debugRow.removeFromLeft(gap);

    requestRPDATAButton.setBounds(
        debugRow.removeFromLeft(120)
    );
#else
    requestButton.setBounds(
        area.removeFromTop(32)
    );

    requestRPDATAButton.setBounds(
        area.removeFromTop(32)
    );
#endif

    auto left = area.removeFromLeft(350);

    auto keygroupSection =
        left.removeFromTop(220)
        .reduced(10);

    // �Œ�w�b�_
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

    // �o�[���������X�N���[��
    keygroupMapViewport.setBounds(
        keygroupSection
    );

    left.removeFromTop(6);

    programTree.setBounds(
        left.reduced(10)
    );

    editorTabs.setBounds(area);

    // Keygroup Editor ��Viewport���ŃX�N���[��
    keyGroupEditor.setSize(
        juce::jmax(
            300,
            keyGroupViewport.getWidth() - 15
        ),
        1200
    );

    // Sample Header��Viewport��
    sampleHeaderEditor.setSize(
        juce::jmax(
            300,
            sampleHeaderViewport.getWidth() - 15
        ),
        750
    );

    // Program Editor��Viewport���ŃX�N���[��
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
            juce::Colours::lightgreen
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

//void MainComponent::handleIncomingMidiMessage(
//    juce::MidiInput*,
//    const juce::MidiMessage& message)
//{
//    if (!message.isSysEx())
//        return;
//
//    /*lastDeviceResponseTime =
//        juce::Time::getMillisecondCounterHiRes();*/
//
//    processIncomingSysEx(message);
//
//    //juce::MessageManager::callAsync(
//    //    [this]()
//    //    {
//    //        deviceConnected = true;
//
//    //        deviceStatusLabel.setText(
//    //            "S3000XL: Connected",
//    //            juce::dontSendNotification
//    //        );
//
//    //        deviceStatusLabel.setColour(
//    //            juce::Label::textColourId,
//    //            juce::Colours::lightgreen
//    //        );
//    //    }
//    //);
//}

void MainComponent::handleIncomingMidiMessage(
    juce::MidiInput*,
    const juce::MidiMessage& message)
{
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


                    if (selectedProgram != currentProgram)
                        loadProgram(selectedProgram);
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
                }

                if (offset == KeygroupHeaderOffset::Mod::Filter3
                    && keygroup >= 0
                    && keygroup < static_cast<int>(loadedProgram.keygroups.size()))
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


    // �܂�ID���m��Ƃ���parse
    SampleHeader sh =
        SampleHeaderParser::parse(
            decoded,
            -1
        );

    // ���ۂɕԂ��Ă���Sample������ID������
    const int resolvedSampleId =
        findSampleId(
            sh.name.trim()
        );



    if (resolvedSampleId < 0)
    {
        DBG(
            "FAILED TO RESOLVE SAMPLE HEADER ID"
        );

        return;
    }

    sh.id = resolvedSampleId;




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

    // ==============================
    // Sample Header��E
    // ==============================


    // ==============================
    // �SSample Header��M�����H
    // ==============================

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

    sysExSender.sendKeygroupHeaderByteRequest(
        loadedProgram.programNumber,
        loadingKeygroup,
        KeygroupHeaderOffset::Mod::Lfo1Pitch
    );

    sysExSender.sendKeygroupHeaderByteRequest(
        loadedProgram.programNumber,
        loadingKeygroup,
        KeygroupHeaderOffset::Mod::Filter1
    );

    sysExSender.sendKeygroupHeaderByteRequest(
        loadedProgram.programNumber,
        loadingKeygroup,
        KeygroupHeaderOffset::Mod::Filter2
    );

    sysExSender.sendKeygroupHeaderByteRequest(
        loadedProgram.programNumber,
        loadingKeygroup,
        KeygroupHeaderOffset::Mod::Filter3
    );




    // ========================================
  // ���񃍁[�h����Map / Tree�𒀎��X�V
  // ========================================

    const int mapUpdateIndex =
        loadingKeygroup;

    const Program programForMap =
        loadedProgram;



    juce::MessageManager::callAsync(
        [this, programForMap, mapUpdateIndex]()
        {


            // KeygroupMap
            keygroupMap.setProgram(
                programForMap
            );

            // ProgramTree
            programTree.setProgram(
                programForMap,
                sampleHeaders
            );


        }
    );


    // ========================================
    // Request Sample Headers
    // ========================================

    for (const auto& zone :
        loadedProgram.keygroups[loadingKeygroup].zones)
    {
        if (zone.sampleId < 0)
            continue;

        // ���łɎ����Ă���Sample Header�Ȃ�
        // �ă��N�G�X�g���Ȃ�
        if (sampleHeaders.find(zone.sampleId)
            != sampleHeaders.end())
        {
            DBG(
                "SAMPLE HEADER ALREADY EXISTS ID="
                + juce::String(zone.sampleId)
            );

            continue;
        }





        sendSampleHeader(
            zone.sampleId
        );
    }





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



    if (!pendingSampleRequests.empty())
    {
        DBG(
            "WAITING FOR SAMPLE HEADERS. pending="
            + juce::String(
                (int)pendingSampleRequests.size()
            )
        );
    }

    // ========================================
// Continue loading Keygroups
// ========================================

    ++loadingKeygroup;

    if (loadingKeygroup < totalKeygroups)
    {
        DBG(
            "REQUEST NEXT KG HEADER INDEX="
            + juce::String(loadingKeygroup)
        );

        sysExSender.sendKGHeader(
            loadedProgram.programNumber,
            loadingKeygroup
        );
    }
    else
    {
        static int finishCount = 0;
        ++finishCount;

        const int restoreKeygroup =
            juce::jlimit(
                0,
                juce::jmax(0, totalKeygroups - 1),
                currentKeygroup
            );

        DBG(
            "RESTORE HARDWARE KG DISPLAY INDEX="
            + juce::String(restoreKeygroup)
        );

        restoringKeygroupDisplay = true;

        sysExSender.sendKGHeader(
            loadedProgram.programNumber,
            restoreKeygroup
        );



        // �ʐM���������Z�b�g
        loadingKeygroup = 0;

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

                // ��ɍŏI�T�C�Y���m��
                keygroupMap.setSize(
                    juce::jmax(
                        1,
                        keygroupMapViewport.getWidth() - 16
                    ),
                    contentHeight
                );

                // ���̌�Program��n��
                keygroupMap.setProgram(
                    loadedProgram
                );

                programTree.setProgram(
                    loadedProgram,
                    sampleHeaders
                );
            }

        );
    }


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

        // MIDI callback����UI�𒼐ڐG��Ȃ�
        juce::MessageManager::callAsync(
            [this, addedIndex]()
            {
                // ========================================
                // ���[�J��Program�X�V
                // ========================================

                loadedProgram.keygroups.push_back(
                    pendingAddedKeygroup
                );

                loadedProgram.groups =
                    static_cast<int>(
                        loadedProgram.keygroups.size()
                        );

                totalKeygroups =
                    loadedProgram.groups;

                currentKeygroup =
                    addedIndex;

                currentZone = 0;

                DBG(
                    "ADD LOCAL UPDATE GROUPS="
                    + juce::String(
                        loadedProgram.groups
                    )
                );

                constexpr int rowHeightForMap = 32;
                constexpr int extraHeight = 80;

                const int contentHeight =
                    juce::jmax(
                        200,
                        static_cast<int>(
                            loadedProgram.keygroups.size()
                            ) * rowHeightForMap
                        + extraHeight
                    );

                keygroupMap.setSize(
                    juce::jmax(
                        1,
                        keygroupMapViewport.getWidth() - 16
                    ),
                    contentHeight
                );


                // ========================================
                // UI�����X�V
                // ========================================

                //keygroupMap.setProgram(
                //    loadedProgram
                //);

                //programTree.setProgram(
                //    loadedProgram,
                //    sampleHeaders
                //);

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
        DBG("RESTORE KG DISPLAY RESPONSE - IGNORE KDATA REQUEST");

        restoringKeygroupDisplay = false;
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

    DBG(
        "REQUEST KDATA INDEX="
        + juce::String(loadingKeygroup)
    );

    sysExSender.sendKData(
        loadedProgram.programNumber,
        loadingKeygroup
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
    // ���łɑ҂��Ă���Ȃ�d��request���Ȃ�
    if (pendingSampleRequests.find(sampleId)
        != pendingSampleRequests.end())
    {
        DBG(
            "SAMPLE HEADER ALREADY PENDING ID="
            + juce::String(sampleId)
        );

        return;
    }

    pendingSampleRequests.insert(sampleId);

    sysExSender.sendSampleHeader(sampleId);
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
            programCombo.setSelectedId(
                1,
                juce::sendNotification
            );
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


// write-test
