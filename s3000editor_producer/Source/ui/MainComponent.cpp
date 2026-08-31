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

    // printable ASCII fallback（超重要・
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
            // 4 Zone全部をOverviewへ
            // ========================================

            velocityZoneEditor.setZones(
                kg.zones
            );

            // ========================================
            // Zone 1を詳細Editorへ
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
        false,   // vertical
        false    // horizontal
    );

    programViewport.setViewedComponent(
        &programEditor,
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
            auto encodedProgram =
                ProgramEncoder::encode(
                    program
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


            sysExSender.sendProgramData(
                program.programNumber,
                encodedProgram
            );

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
        };

    //addAndMakeVisible(programTree);

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

            // 選択したZoneを詳細Editorへ
            velocityZoneEditor.setZone(
                kg.zones[currentZone]
            );

            // Sample Headerも切り替える
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
            currentZone = zoneIndex;

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
            // 4 Zone全部をOverviewへ
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
            // 選択Zoneを詳細Editorへ
            // ========================================

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

    // デバッグ用：表示領域を確認
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

            // Velocity Zoneは必ず表示
            velocityZoneEditor.setZone(zone);

            // ==============================
            // Sample HeaderもIDから探す
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

            // Keygroupを選択したらZone 1を選択状態にする
            currentZone = 0;

            editorTabs.setCurrentTabIndex(0);

            keyGroupEditor.setKeygroup(
                keygroup,
                keygroupIndex
            );

            // ========================================
            // 4 Zone Overview更新
            // ========================================

            velocityZoneEditor.setZones(
                keygroup.zones
            );

            velocityZoneEditor.setSelectedZone(
                currentZone
            );

            // ========================================
            // Zone 1を詳細Editorにもセット
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

            DBG(
                "PROGRAM SELECTED INDEX="
                + juce::String(programIndex)
            );

            DBG(
                "PROGRAM SELECTED NAME=["
                + programCombo.getText()
                + "]"
            );

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

            // 選択中のKGならEditorにも即反映
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

            loadedProgram.keygroups[keygroupIndex] =
                keygroup;

            // Keygroup Mapにも変更を反映
            keygroupMap.setProgram(
                loadedProgram
            );

            DBG("LOADED PROGRAM KEYGROUP UPDATED");

            auto& updatedKeygroup =
                loadedProgram.keygroups[keygroupIndex];

            auto encoded =
                KeygroupEncoder::encode(
                    updatedKeygroup
                );

            if (encoded.empty())
            {
                DBG("KEYGROUP ENCODE FAILED");
                return;
            }

            // 差分確認
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
                loadedProgram.programNumber,
                keygroupIndex,
                encoded
            );

        };


    velocityZoneEditor.onZoneChanged =
        [this](const VelocityZone& zone)
        {
            auto& kg =
                loadedProgram.keygroups[
                    currentKeygroup
                ];

            // ========================================
            // ローカルZone更新
            // ========================================

            kg.zones[currentZone] =
                zone;

            // ========================================
            // 4-Zone Overviewも即更新
            // ========================================

            velocityZoneEditor.setZones(
                kg.zones
            );

            // 選択ハイライトも維持
            velocityZoneEditor.setSelectedZone(
                currentZone
            );

            // ========================================
            // 実機へKDATA
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
                loadedProgram.programNumber,
                currentKeygroup,
                encoded
            );
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

            // ローカルデータ更新
            sampleHeaders[header.id] = header;

            // Sample Header -> SysEx
            auto sysex =
                SampleHeaderEncoder::makeSysEx(
                    header
                );

            DBG(
                "SYSEX SIZE = "
                + juce::String(
                    (int)sysex.size()
                )
            );

            sysExSender.sendSysEx(sysex);



            DBG("SAMPLE HEADER SYSEX SENT");
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

    programCombo.setBounds(
        topBar.removeFromLeft(290)
        .reduced(20, 15)
    );

    deviceStatusLabel.setBounds(
        topBar.removeFromRight(250)
        .reduced(10, 15)
    );


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

    // 固定ヘッダ
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

    // バー部分だけスクロール
    keygroupMapViewport.setBounds(
        keygroupSection
    );

    left.removeFromTop(6);

    programTree.setBounds(
        left.reduced(10)
    );

    editorTabs.setBounds(area);

    // Keygroup Editor はViewport内でスクロール
    keyGroupEditor.setSize(
        juce::jmax(
            300,
            keyGroupViewport.getWidth() - 15
        ),
        1000
    );

    // Sample HeaderもViewport内
    sampleHeaderEditor.setSize(
        juce::jmax(
            300,
            sampleHeaderViewport.getWidth() - 15
        ),
        750
    );

    // Program EditorもViewport内でスクロール
    programEditor.setSize(
        juce::jmax(
            300,
            programViewport.getWidth() - 15
        ),
        1250
    );

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

    if (heartbeatCounter >= 3)
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
            programRefreshPending = false;
            programRefreshOnly = true;

            DBG("PROGRAM REFRESH ONLY REQUEST");

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
                // アプリ起動後の最初の接続確認
                // 起動時の AUTO RPLIST がすでに走っている
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
    juce::MidiInput*,
    const juce::MidiMessage& message)
{
    if (!message.isSysEx())
        return;

    /*lastDeviceResponseTime =
        juce::Time::getMillisecondCounterHiRes();*/

    processIncomingSysEx(message);

    //juce::MessageManager::callAsync(
    //    [this]()
    //    {
    //        deviceConnected = true;

    //        deviceStatusLabel.setText(
    //            "S3000XL: Connected",
    //            juce::dontSendNotification
    //        );

    //        deviceStatusLabel.setColour(
    //            juce::Label::textColourId,
    //            juce::Colours::lightgreen
    //        );
    //    }
    //);
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

    // MDATA response = heartbeat response
    if (opcode == 0x11)
    {
        lastDeviceResponseTime =
            juce::Time::getMillisecondCounterHiRes();

        juce::MessageManager::callAsync(
            [this]()
            {
                setDeviceConnected(true);
            }
        );

        DBG("HEARTBEAT MDATA RECEIVED");
        return;
    }

    DBG("=== INCOMING SYSEX ===");
    DBG("opcode = 0x" + juce::String::toHexString(opcode));
    DBG("getSysExDataSize = " + juce::String((int)size));
    DBG("message raw size = " + juce::String(message.getRawDataSize()));

    programBuffer.reset();
    programBuffer.append(data, size);

    static int captureIndex = 0;

    //juce::String name =
    //    (captureIndex == 0)
    //    ? "rpdata_A"
    //    : "rpdata_B";

    //saveRawRPDATA(name, programBuffer);

//    std::vector<uint8_t> decoded;

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
            // 実機のパラメータ変更通知
            if (size < 395)
            {
                lastShortProgramChangeTime =
                    juce::Time::getMillisecondCounterHiRes();

                programRefreshPending = true;
                break;
            }

            // Full Program Header
            handleProgramHeaderResponse();
            break;
        }
            
        case 0x2A:
        {
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
    DBG("SAMPLE HEADER RECEIVED");
    DBG("RAW SAMPLE HEADER SYSEX");

    auto* p =
        (uint8_t*)programBuffer.getData();

    const int rawSize =
        (int)programBuffer.getSize();

    // ==============================
    // Raw SysEx先頭確認
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
    // Raw SysEx末尾確認
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

    DBG(
        "FULL SAMPLE HEADER SIZE = "
        + juce::String((int)decoded.size())
    );
    
    if (decoded.size() < 141)
    {
        DBG("INVALID SAMPLE HEADER SIZE");
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
    // Decode先頭確認
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
    // pending request確認
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
    // 対象Sample IDを取得
    // ==============================

    //const int sampleId =
    //    pendingSampleRequests.front();

    //pendingSampleRequests.pop();

    //DBG(
    //    "PENDING SAMPLE ID = "
    //    + juce::String(sampleId)
    //);

    // ==============================
    // Parse
    // ==============================

    //SampleHeader sh =
    //    SampleHeaderParser::parse(
    //        decoded,
    //        sampleId
    //    );

    // まずID未確定としてparse
    SampleHeader sh =
        SampleHeaderParser::parse(
            decoded,
            -1
        );

    // 実際に返ってきたSample名からIDを解決
    const int resolvedSampleId =
        findSampleId(
            sh.name.trim()
        );

    DBG(
        "RECEIVED SAMPLE NAME=["
        + sh.name
        + "]"
    );

    DBG(
        "RESOLVED SAMPLE ID = "
        + juce::String(resolvedSampleId)
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
    
    DBG("=== SAMPLE HEADER VERIFY ===");

    DBG("ID = " + juce::String(sh.id));
    DBG("NAME = [" + sh.name + "]");
    DBG("PITCH = " + juce::String(sh.originalPitch));
    DBG("TUNE = " + juce::String(sh.tune, 2));

    DBG(
        "SAMPLE RATE = "
        + juce::String(sh.sampleRate)
    );

    DBG(
        "SAMPLE RATE VALID = "
        + juce::String(sh.sampleRateValid)
    );

    DBG("NUM LOOPS = " + juce::String(sh.numLoops));
    DBG("ACTIVE LOOP = " + juce::String(sh.activeLoop));
    DBG("PLAY TYPE = " + juce::String(sh.playType));
    DBG(
        "NUM LOOPS = "
        + juce::String(sh.numLoops)
    );

    DBG(
        "ACTIVE LOOP = "
        + juce::String(sh.activeLoop)
    );

    DBG(
        "PLAY TYPE = "
        + juce::String(sh.playType)
    );

    DBG(
        "START = "
        + juce::String(sh.start)
    );

    DBG(
        "END = "
        + juce::String(sh.end)
    );

    DBG(
        "HOLD LOOP TUNE = "
        + juce::String((int)sh.holdLoopTune)
    );

    DBG(
        "LOOP1 POSITION = "
        + juce::String(sh.loops[0].position)
    );

    DBG(
        "LOOP1 LENGTH = "
        + juce::String(sh.loops[0].length, 3)
    );

    DBG(
        "LOOP1 DWELL = "
        + juce::String(sh.loops[0].dwell)
    );
    
    
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


    DBG(
        "STORED SAMPLE HEADER ID="
        + juce::String(sh.id)
        + " NAME=["
        + sh.name
        + "]"
    );

    DBG(
        "sampleHeaders.size = "
        + juce::String(
            (int)sampleHeaders.size()
        )
    );

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
    // Sample Header情・
    // ==============================

    DBG("=== SAMPLE HEADER ===");

    DBG(
        "ID = "
        + juce::String(sh.id)
    );

    DBG(
        "NAME = "
        + sh.name
    );

    DBG(
        "PITCH = "
        + juce::String(sh.originalPitch)
    );

    DBG(
        "LENGTH = "
        + juce::String(
            (juce::int64)sh.length
        )
    );

    DBG(
        "LOOPS = "
        + juce::String(sh.numLoops)
    );

    DBG(
        "PLAY TYPE = "
        + juce::String(sh.playType)
    );
    
    // ==============================
    // 全Sample Header受信完了？
    // ==============================

    if (pendingSampleRequests.empty())
    {
        DBG("ALL SAMPLE HEADERS RECEIVED");

        juce::MessageManager::callAsync(
            [this]
            {
                DBG("CALL ASYNC START");

                // Sample Name -> Sample ID 再解決
                resolveAllSampleIds();

                DBG(
                    "AFTER resolveAllSampleIds"
                );

                // 確認ログ
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

                // 最新データでTree再構築
                programTree.setProgram(
                    loadedProgram,
                    sampleHeaders
                );

                DBG(
                    "PROGRAM TREE UPDATEd "
                    "AFTER ALL SAMPLE HEADERS"
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
    DBG("SLIST RECEIVED");
    DBG("BEFORE parseSLIST");

    parseSLIST(programBuffer);

    DBG("AFTER parseSLIST");

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

    DBG("KEYGROUP DATA RECEIVED");

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
        "RELOADED KG INDEX = "
        + juce::String(loadingKeygroup)
    );

    DBG(
        "RELOADED LOW = "
        + juce::String(
            (int)decoded[
                KeygroupHeaderOffset::Common::LONOTE
            ]
        )
    );

    DBG(
        "RELOADED HIGH = "
        + juce::String(
            (int)decoded[
                KeygroupHeaderOffset::Common::HINOTE
            ]
        )
    );

    saveDecodedDump(
        "keygroup_basic",
        decoded
    );

    DBG(
        "FIXTURE SIZE = "
        + juce::String((int)decoded.size())
    );


    DBG(
        "ZONE1 LOW XFADE RAW = "
        + juce::String((int)decoded[78])
    );

    DBG(
        "ZONE1 HIGH XFADE RAW = "
        + juce::String((int)decoded[79])
    );


    DBG(
        "RELOADED RAW LONOTE = "
        + juce::String(
            (int)decoded[
                KeygroupHeaderOffset::Common::LONOTE
            ]
        )
    );

    DBG(
        "RELOADED RAW HINOTE = "
        + juce::String(
            (int)decoded[
                KeygroupHeaderOffset::Common::HINOTE
            ]
        )
    );


    DBG("===== FULL KEYGROUP DECODED =====");

    for (int i = 0; i < decoded.size(); ++i)
    {
        DBG(
            juce::String(i)
            + " : 0x"
            + juce::String::toHexString((int)decoded[i])
            + " ("
            + juce::String((int)decoded[i])
            + ")"
        );
    }

    DBG("=================================");

    DBG(
        "KG FULL SIZE = "
        + juce::String((int)decoded.size())
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

    DBG("=== ENV2 ===");

    DBG("R1 = " + juce::String(kg.env2.r1));
    DBG("L1 = " + juce::String(kg.env2.l1));

    DBG("R2 = " + juce::String(kg.env2.r2));
    DBG("L2 = " + juce::String(kg.env2.l2));

    DBG("R3 = " + juce::String(kg.env2.r3));
    DBG("L3 = " + juce::String(kg.env2.l3));

    DBG("R4 = " + juce::String(kg.env2.r4));
    DBG("L4 = " + juce::String(kg.env2.l4));

    DBG("=== ENV2 RAW ===");

    DBG("d[20]  R1 = " + juce::String((int)decoded[20]));
    DBG("d[156] L1 = " + juce::String((int)decoded[156]));
    DBG("d[157] R2 = " + juce::String((int)decoded[157]));
    DBG("d[158] L2 = " + juce::String((int)decoded[158]));
    DBG("d[21]  R3 = " + juce::String((int)decoded[21]));
    DBG("d[22]  L3 = " + juce::String((int)decoded[22]));
    DBG("d[23]  R4 = " + juce::String((int)decoded[23]));
    DBG("d[159] L4 = " + juce::String((int)decoded[159]));

    DBG("TUNE = " + juce::String(kg.tune));

    DBG("FILTER KEY FOLLOW = "
        + juce::String(kg.filter.keyFollow));

    DBG("VELOCITY TO FREQ = "
        + juce::String(kg.filter.velocityToFreq));

    DBG("PRESSURE TO FREQ = "
        + juce::String(kg.filter.pressureToFreq));

    DBG("ENVELOPE TO FREQ = "
        + juce::String(kg.filter.envelopeToFreq));

    DBG("ENV1 SUSTAIN = "
        + juce::String(kg.env1.sustain));

    DBG("ENV1 RELEASE = "
        + juce::String(kg.env1.release));

    DBG("ENV2 VEL ATTACK = "
        + juce::String(kg.env2.velAttack));

    DBG("ENV2 VEL RELEASE = "
        + juce::String(kg.env2.velRelease));

    DBG("ENV2 NOTE-OFF RELEASE = "
        + juce::String(kg.env2.noteOffRelease));

    DBG("ENV2 KEY TRACKING = "
        + juce::String(kg.env2.keyTracking));

    DBG("E_PTCH = "
        + juce::String(kg.velocity.ePtch));

    DBG("VXFADE = "
        + juce::String(kg.velocity.vxFade));

    DBG("VZONES = "
        + juce::String(kg.velocity.vZones));



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



    DBG(
        "STORED KEYGROUP INDEX = "
        + juce::String(loadingKeygroup)
    );

    DBG(
        "AFTER STORE loadedProgram.keygroups.size = "
        + juce::String(
            (int)loadedProgram.keygroups.size()
        )
    );

    // ========================================
  // 初回ロード中もMap / Treeを逐次更新
  // ========================================

    const int mapUpdateIndex =
        loadingKeygroup;

    const Program programForMap =
        loadedProgram;

    DBG(
        "QUEUE MAP UPDATE KG="
        + juce::String(mapUpdateIndex)
    );

    juce::MessageManager::callAsync(
        [this, programForMap, mapUpdateIndex]()
        {
            DBG(
                "QUEUE MAP UPDATE KG="
                + juce::String(mapUpdateIndex)
                + " PROGRAM KG SIZE="
                + juce::String(
                    (int)programForMap.keygroups.size()
                )
            );

            // KeygroupMap
            keygroupMap.setProgram(
                programForMap
            );

            // ProgramTree
            programTree.setProgram(
                programForMap,
                sampleHeaders
            );

            DBG(
                "PARTIAL MAP + TREE UPDATE KG="
                + juce::String(mapUpdateIndex)
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

        // すでに持っているSample Headerなら
        // 再リクエストしない
        if (sampleHeaders.find(zone.sampleId)
            != sampleHeaders.end())
        {
            DBG(
                "SAMPLE HEADER ALREADY EXISTS ID="
                + juce::String(zone.sampleId)
            );

            continue;
        }



        DBG(
            "REQUEST SAMPLE HEADER ID="
            + juce::String(zone.sampleId)
            + " NAME=["
            + zone.sampleName
            + "]"
        );

        sendSampleHeader(
            zone.sampleId
        );
    }


    DBG(
        "PENDING SAMPLE REQUESTS = "
        + juce::String(
            (int)pendingSampleRequests.size()
        )
    );


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

        DBG(
            "KEYGROUP "
            + juce::String(k)
        );

        for (int z = 0;
            z < storedKg.zones.size();
            ++z)
        {
            const auto& zone =
                storedKg.zones[z];

            DBG(
                "ZONE "
                + juce::String(z)
                + " SAMPLE=["
                + zone.sampleName
                + "] ID="
                + juce::String(zone.sampleId)
            );
        }
    }


    // ========================================
    // Sample Header requestが無い場合だけ
    // ここでTreeを更新
    //
    // test
    // 全Header受信後にTree更新
    // ========================================

    // ========================================
// Keygroupを受信した時点でTreeを表示
// Sample Headerが未取得でも構わない
// ========================================

    //juce::MessageManager::callAsync(
    //    [this]
    //    {
    //        DBG("UPDATE TREE FROM KEYGROUP");

    //        programTree.setProgram(
    //            loadedProgram,
    //            sampleHeaders
    //        );

    //        keygroupMap.setProgram(
    //            loadedProgram
    //        );

    //        // =====================================
    //        // Keygroup Mapのコンテンツサイズ
    //        // =====================================

    //        constexpr int rowHeight = 32;

    //        const int contentHeight =
    //            juce::jmax(
    //                200,
    //                static_cast<int>(
    //                    loadedProgram.keygroups.size()
    //                    ) * rowHeight
    //            );

    //        keygroupMap.setSize(
    //            juce::jmax(
    //                1,
    //                keygroupMapViewport.getWidth() - 16
    //            ),
    //            contentHeight
    //        );

    //        // 以下、今までの処理
    //        if (!loadedProgram.keygroups.empty())
    //        {
    //            auto& kg =
    //                loadedProgram.keygroups[0];

    //            keyGroupEditor.setKeygroup(
    //                kg,
    //                0
    //            );

    //            if (!kg.zones.empty())
    //            {
    //                currentZone = 0;

    //                velocityZoneEditor.setZone(
    //                    kg.zones[0]
    //                );
    //            }
    //        }

    //        DBG(
    //            "PROGRAM TREE + KEYGROUP MAP + EDITORS "
    //            "UPDATED FROM KEYGROUP"
    //        );
    //    }
    //
    //);
  

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

        DBG(
            "!!!!!!!! KG LOAD FINISHED #"
            + juce::String(finishCount)
            + " loadingKeygroup="
            + juce::String(loadingKeygroup)
            + " total="
            + juce::String(totalKeygroups)
        );

        DBG(
            "ALL KEYGROUPS LOADED count="
            + juce::String(totalKeygroups)
        );

        // 通信側だけリセット
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

                // 先に最終サイズを確定
                keygroupMap.setSize(
                    juce::jmax(
                        1,
                        keygroupMapViewport.getWidth() - 16
                    ),
                    contentHeight
                );

                // その後Programを渡す
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
        // GROUPS + 1 のPDATAを作る
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

        // MIDI callbackからUIを直接触らない
        juce::MessageManager::callAsync(
            [this, addedIndex]()
            {
                // ========================================
                // ローカルProgram更新
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
                // UIだけ更新
                // ========================================

                keygroupMap.setProgram(
                    loadedProgram
                );

                programTree.setProgram(
                    loadedProgram,
                    sampleHeaders
                );

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

                DBG(
                    "ADD LOCAL UI UPDATE COMPLETE"
                );
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
                    // Model更新
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


                    DBG(
                        "DELETE LOCAL UPDATE GROUPS="
                        + juce::String(
                            loadedProgram.groups
                        )
                    );

                    // =========================
                    // 選択位置を補正
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
                    // UI更新
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
    DBG("========== CASE 0x28 ENTERED ==========");

    DBG(
        "programBuffer SIZE BEFORE DECODE = "
        + juce::String((int)programBuffer.getSize())
    );

    auto decoded =
        decodeProgramHeader(programBuffer);

    DBG(
        "PROGRAM SIZE = "
        + juce::String((int)decoded.size())
    );

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

    loadedProgram.keygroups =
        std::move(existingKeygroups);

    loadedProgram.rawData = decoded;

    programBuffer.reset();


    // ========================================
    // Program Editor は常に更新
    // ========================================

    const Program programForUI =
        loadedProgram;

    juce::MessageManager::callAsync(
        [this, programForUI]()
        {
            programEditor.setProgram(
                programForUI
            );

            DBG(
                "PROGRAM EDITOR SET FROM PROGRAM HEADER"
            );
        }
    );


    // ========================================
    // 実機パラメータ変更による再取得なら
    // 左側は一切更新しない
    // ========================================

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
    // ここから通常の Program Load
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

            DBG(
                "INITIAL MAP SIZE SET "
                + juce::String(keygroupMap.getWidth())
                + " x "
                + juce::String(keygroupMap.getHeight())
            );
        }
    );

    DBG(
        "TOTAL KEYGROUPS = "
        + juce::String(totalKeygroups)
    );


    // ========================================
    // Request first Keygroup
    // ========================================

    if (totalKeygroups > 0)
    {
        DBG(
            "REQUEST KG HEADER INDEX="
            + juce::String(loadingKeygroup)
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
    // 最後のKGをコピーして新KGを作る
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
    // Reply後に必要なので保存
    // ========================================

    pendingAddedKeygroup =
        newKg;

    pendingAddKeygroupIndex =
        newKeygroupIndex;

    pendingAddStage =
        PendingAddStage::
        waitingForKeygroupReply;

    DBG("===== ADD KEYGROUP =====");

    DBG(
        "GROUPS "
        + juce::String(oldGroups)
        + " -> "
        + juce::String(oldGroups + 1)
    );

    DBG(
        "STEP 1: SEND KDATA KG="
        + juce::String(newKeygroupIndex)
    );

    // ========================================
    // STEP 1
    // KDATAだけ送る
    // ========================================

    sysExSender.sendKeygroupData(
        loadedProgram.programNumber,
        newKeygroupIndex,
        encodedKg
    );
}

void MainComponent::handleKeygroupFullResponse()
{
    DBG("KEYGROUP DATA RECEIVED");

    auto decoded =
        decodeKeygroupFull(programBuffer);

    DBG(
        "FULL KG decoded size = "
        + juce::String((int)decoded.size())
    );

    DBG("=== KEYGROUP FULL DATA ===");
}

void MainComponent::handleRPDataResponse()
{
    DBG("RPDATA RESPONSE RECEIVED");

    std::vector<uint8_t> decoded;

    parseRPDATA(decoded);
}


//void MainComponent::handleKeygroupHeaderResponse()
//{
//    DBG("ENTER CASE 0x2A");
//
//    auto decoded =
//        decodeKeygroupHeader(programBuffer);
//
//    DBG(
//        "decodeKeygroupHeader size = "
//        + juce::String((int)decoded.size())
//    );
//
//    Keygroup kg =
//        KeygroupParser::parse(
//            decoded,
//            residentSamples
//        );
//
//    for (auto& zone : kg.zones)
//    {
//        if (zone.sampleName.isEmpty())
//            continue;
//
//        zone.sampleId =
//            findSampleId(
//                juce::String(zone.sampleName)
//            );
//
//        DBG(
//            "Resolved "
//            + juce::String(zone.sampleName)
//            + " -> "
//            + juce::String(zone.sampleId)
//        );
//    }
//
//    if (loadingKeygroup >=
//        (int)loadedProgram.keygroups.size())
//    {
//        loadedProgram.keygroups.resize(
//            loadingKeygroup + 1
//        );
//    }
//
//    loadedProgram.keygroups[loadingKeygroup] = kg;
//
//    sysExSender.sendRSLIST();
//
//    sysExSender.sendKData(
//        loadedProgram.programNumber,
//        loadingKeygroup
//    );
//
//    DBG("ENV1 SUSTAIN = "
//        + juce::String(kg.env1.sustain));
//
//    DBG("ENV1 RELEASE = "
//        + juce::String(kg.env1.release));
//}

void MainComponent::handleKeygroupHeaderResponse()
{
    DBG("ENTER CASE 0x2A");

    auto decoded =
        decodeKeygroupHeader(programBuffer);

    DBG(
        "decodeKeygroupHeader size = "
        + juce::String((int)decoded.size())
    );

    // 0x2AではloadedProgramを更新しない。
    // 完全なKeygroup情報は0x09 KDATAで取得する。

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

    DBG("===== findSampleId =====");

    DBG(
        "TARGET=["
        + target
        + "]"
    );

    DBG(
        "residentSamples.size = "
        + juce::String(
            (int)residentSamples.size()
        )
    );

    for (const auto& s : residentSamples)
    {
        const auto candidate =
            s.second.trim();

        DBG(
            "ID="
            + juce::String(s.first)
            + " NAME=["
            + candidate
            + "]"
        );

        if (candidate == target)
        {
            DBG(
                "MATCHED ID="
                + juce::String(s.first)
            );

            return s.first;
        }
    }

    DBG(
        "NO MATCH FOR ["
        + target
        + "]"
    );

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

            DBG(
                "RESOLVE SAMPLE = "
                + zone.sampleName
                + " ID = "
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


    DBG("Program decoded size = "
        + juce::String((int)decoded.size()));


    return decoded;
}


void MainComponent::parseSLIST(
    const juce::MemoryBlock& data)
{
    const auto dataSize =
        static_cast<int>(data.getSize());

    DBG("=== parseSLIST ===");
    DBG("SLIST DATA SIZE = "
        + juce::String(dataSize));

    // countを読むには最低6 bytes必要
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

    DBG("SLIST REQUIRED SIZE = "
        + juce::String(requiredSize));

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

    DBG(
        "RESIDENT SAMPLE SIZE = "
        + juce::String(
            residentSamples.size()
        )
    );

    const auto samplesCopy =
        residentSamples;

    juce::MessageManager::callAsync(
        [this, samplesCopy]()
        {
            DBG(
                "SETTING RESIDENT SAMPLES TO "
                "VELOCITY ZONE EDITOR"
            );

            velocityZoneEditor.setResidentSamples(
                samplesCopy
            );

            DBG(
                "RESIDENT SAMPLES SET TO "
                "VELOCITY ZONE EDITOR"
            );
        }
    );
}




void MainComponent::saveDump(const juce::String& name)
{
    DBG("Saving size = "
        + juce::String((int)sysExBuffer.getSize()));
    auto file = juce::File::getSpecialLocation(
        juce::File::userDesktopDirectory)
        .getChildFile(name + ".bin");

    file.replaceWithData(sysExBuffer.getData(),
        sysExBuffer.getSize());

    DBG("Saved: " + file.getFullPathName());

}

void MainComponent::saveDecodedDump(const juce::String& name,
    const std::vector<uint8_t>& decoded)
{

    auto file = juce::File::getSpecialLocation(
        juce::File::userDesktopDirectory)
        .getChildFile(name + "_decoded.bin");

    file.replaceWithData(decoded.data(),
        decoded.size());

    DBG("Saved: " + file.getFullPathName());

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

    DBG("=== DEBUG RAW CHECK ===");

    DBG("A size = " + juce::String((int)dataA.getSize()));
    DBG("B size = " + juce::String((int)dataB.getSize()));

    DBG("A ptr = " + juce::String((uintptr_t)dataA.getData()));
    DBG("B ptr = " + juce::String((uintptr_t)dataB.getData()));

    DBG("A first 16 bytes:");
    for (int i = 0; i < 16; ++i)
        DBG(juce::String::toHexString(((uint8_t*)dataA.getData())[i]));

    DBG("B first 16 bytes:");
    for (int i = 0; i < 16; ++i)
        DBG(juce::String::toHexString(((uint8_t*)dataB.getData())[i]));


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

    DBG("=== END ===");
    DBG("A size = "
        + juce::String((int)dataA.getSize()));

    DBG("B size = "
        + juce::String((int)dataB.getSize()));



}



//void MainComponent::sendSampleHeader(int sampleId)
//{
//    pendingSampleRequests.push(sampleId);
//
//    sysExSender.sendSampleHeader(sampleId);
//}

void MainComponent::sendSampleHeader(int sampleId)
{
    // すでに待っているなら重複requestしない
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




    DBG("DECODED SIZE = "
        + juce::String((int)decoded.size()));

    DBG("=== FIRST 32 BYTES ===");

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

    DBG("===== PROGRAM =====");

    DBG("Program Number = "
        + juce::String(program.programNumber));


    DBG("MIDI Channel = "
        + juce::String(program.midiChannel));


    DBG("Polyphony = "
        + juce::String(program.polyphony));


    DBG("Keygroup Count = "
        + juce::String(
            (int)program.keygroups.size()
        ));

    DBG("=== PROGRAM HEX ===");

    for (int i = 0; i < 100; i++)
    {
        DBG(
            juce::String(i)
            + " : "
            + juce::String::toHexString(decoded[i])
        );
    }

    DBG("=== FIRST 64 BYTES ===");

    for (int i = 0; i < 64; i++)
    {
        DBG(
            juce::String(i)
            + " : "
            + juce::String::toHexString(decoded[i])
        );
    }

    DBG("=== FIRST 100 BYTES ===");

    for (int i = 0; i < 100; i++)
    {
        DBG(
            juce::String(i)
            + " : "
            + juce::String::toHexString(decoded[i])
        );
    }


  
}



//void MainComponent::sendRSLIST()
//{
//    DBG("sendRSLIST called");
//
//
//    uint8_t data[]
//    {
//        0x47,   // AKAI manufacturer ID
//        0x00,   // channel
//
//        0x04,   // RSLIST request
//
//        0x48    // S1000 identity
//    };
//
//
//    auto msg =
//        juce::MidiMessage::createSysExMessage(
//            data,
//            sizeof(data));
//
//
//    midiOutput->sendMessageNow(msg);
//
//
//    DBG("RSLIST REQUEST SENT");
//}


void MainComponent::parsePLIST(const std::vector<uint8_t>& d)
{
    programList.clear();
    programCombo.clear(
        juce::dontSendNotification
    );

    int i = 4;

    int count = d[i++];

    DBG("count = " + juce::String(count));

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
        entry.index = p; // ← 仮想index（実機は無視）
        entry.name = name;

        programList.push_back(entry);

        // ComboBoxにも追加
        programCombo.addItem(
            juce::String(name),
            p + 1
        );

        DBG(juce::String(p) + " : " + name);
    }

    if (programCombo.getNumItems() > 0)
    {
        programCombo.setSelectedId(
            1,
            juce::sendNotification
        );
    }

    listBox.updateContent();
    listBox.repaint();

    repaint();
}

//void MainComponent::parsePLIST(const std::vector<uint8_t>& d)
//{
//
//    DBG("First name raw:");
//
//    for (int k = 0; k < 12; ++k)
//    {
//        DBG(juce::String(k) + " = "
//            + juce::String::toHexString((int)d[5 + k]));
//    }
//
//    programList.clear();
//
//    int i = 4;
//
//    if (i >= (int)d.size())
//        return;
//
//
//    int count = d[i++];
//
//    DBG("count = " + juce::String(count));
//
//    for (int p = 0; p < count; ++p)
//    {
//        ProgramEntry entry;
//
//        //entry.index = d[i++];
//        entry.index = p;
//
//        std::string name;
//        name.reserve(16);
//
//        for (int n = 0; n < 12; ++n)
//        {
//            if (i >= (int)d.size())
//                break;
//
//            uint8_t v = d[i++];
//            if (v == 0x00)
//                continue;   // ← これだけ終端扱い
//            char c = decodePlistChar(v);
//            name += c;
//        }
//
//        while (!name.empty() && name.back() == ' ')
//            name.pop_back();
//
//        //entry.name = trimRightSpaces(name);
//        //programList.push_back(entry);
//        DBG(juce::String(p) + " : " + name);
//        //DBG(juce::String(entry.index) + " : " + entry.name);
//    }
//}
//void MainComponent::parsePLIST(const std::vector<uint8_t>& d)
//{
//    programList.clear();
//
//    if (d.size() < 6)
//        return;
//
//    int i = 4;
//
//    int count = d[i++];
//    DBG("count = " + juce::String(count));
//
//    count = juce::jmin(count, 128);
//
//    for (int p = 0; p < count; ++p)
//    {
//        if (i >= (int)d.size())
//            break;
//
//        ProgramEntry entry;
//        entry.index = d[i++];
//
//        std::string name;
//        name.reserve(12);
//
//        // ★完全固定12バイト
//        for (int n = 0; n < 12; ++n)
//        {
//            if (i >= (int)d.size())
//                break;
//
//            uint8_t v = d[i++];
//
//            if (v == 0x0A)
//                name += ' ';
//            else
//                name += decodePlistChar(v);
//        }
//
//        entry.name = trimRightSpaces(name);
//        programList.push_back(entry);
//
//        DBG(juce::String(entry.index) + " : " + entry.name);
//    }
//
//    repaint();
//}

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
    DBG("RAW SIZE = " + juce::String((int)data.getSize()));

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

    DBG("decoded size = " + juce::String((int)decoded.size()));
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
    DBG(
        "========== LOAD PROGRAM CALLED INDEX="
        + juce::String(programIndex)
        + " =========="
    );

    if (programIndex < 0)
        return;

    currentProgram = programIndex;

    currentKeygroup = 0;
    currentZone = 0;

    loadingKeygroup = 0;

    loadedProgram.programNumber =
        programIndex;

    loadedProgram.keygroups.clear();

    sysExSender.sendRSLIST();

    DBG(
        "LOAD PROGRAM INDEX="
        + juce::String(programIndex)
    );

    // Programロード開始
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

    DBG("KG decoded size = "
        + juce::String((int)decoded.size()));

    for (size_t i = 0; i < decoded.size(); i++)
    {
        DBG(
            juce::String((int)i)
            + " : 0x"
            + juce::String::toHexString(decoded[i])
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
    // F0はmessage.getSysExData()には含まれない
    // 47 cc 09 48 pp pp kk の後からdata開始

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
    DBG("SAMPLE HEADER RAW BUFFER SIZE = "
        + juce::String((int)data.getSize()));

    DBG("SAMPLE HEADER DECODE START = 11");

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
    DBG("SAMPLE HEADER DECODED SIZE = "
        + juce::String((int)decoded.size()));

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

    DBG("=== BEFORE PARSE ===");

    for (int i = 0; i < 40; i++)
    {
        DBG(juce::String(i)
            + " : "
            + juce::String::toHexString(decoded[i]));
    }


    Program program = ProgramParser::parse(decoded);


    DBG("===== PROGRAM =====");
    DBG("SIZE   = " + juce::String((int)decoded.size()));
    //DBG("PAN    = " + juce::String(params.pan));
    //DBG("FREQ   = " + juce::String(params.frequency));
    //DBG("FILTER = " + juce::String(params.filter));


}

//void MainComponent::parseKeygroup(
//    const std::vector<uint8_t>& decoded
//)
//{
//    dumpKeygroup(decoded);
//
//    Keygroup k = parseKeygroupStruct(decoded);
//
//   
//}



void MainComponent::saveRawDump(const juce::String& name)
{
    auto file = juce::File::getSpecialLocation(
        juce::File::userDesktopDirectory)
        .getChildFile(name + "_raw.bin");

    file.replaceWithData(sysExBuffer.getData(),
        sysExBuffer.getSize());

    DBG("Saved RAW: " + file.getFullPathName());
}

void MainComponent::saveRawRPDATA(const juce::String& name,
    const juce::MemoryBlock& data)
{
    auto file = juce::File::getSpecialLocation(
        juce::File::userDesktopDirectory)
        .getChildFile(name + "_raw.bin");

    file.replaceWithData(data.getData(), data.getSize());

    DBG("Save Raw SysEx: " + file.getFullPathName());
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
    DBG("========== listBoxItemClicked CALLED ==========");
    DBG("ROW = " + juce::String(row));

    if (row < 0 || row >= programList.size())
    {
        return;
    }


    currentProgramIndex =
        programList[row].index;


    DBG(
        "Selected Program = "
        + juce::String(currentProgramIndex)
    );


//    sendProgramHeader(
//        currentProgramIndex
//    );
    
    sysExSender.sendProgramHeader(currentProgramIndex);
}

// write-test
