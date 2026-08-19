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

char decodeAkaiChar(uint8_t v)
{
    // space / padding
    if (v == 0x0A || v == 0x00)
        return ' ';

    // printable ASCII fallback（超重要）
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



    sampleHeaderViewport.setViewedComponent(
        &sampleHeaderEditor,
        false
    );

    keyGroupViewport.setViewedComponent(
        &keyGroupEditor,
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

    //addAndMakeVisible(programTree);

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

            velocityZoneEditor.setZone(zone);
            sampleHeaderEditor.setSampleHeader(sampleHeader);

            editorTabs.setCurrentTabIndex(1);
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

            if (zone.sampleId >= 0)
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


            editorTabs.setCurrentTabIndex(0);

            keyGroupEditor.setKeygroup(
                keygroup,
                keygroupIndex
            );
        };

    //addAndMakeVisible(keyGroupEditor);

    //addAndMakeVisible(keyGroupViewport);




    //programTree.setProgram(currentProgramData, sampleHeaders);

    //addAndMakeVisible(velocityZoneEditor);
    addAndMakeVisible(captureAButton);

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
        };

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

    requestRPDATAButton.onClick = [this]()
        {
            DBG("========== GET PROGRAM BUTTON CLICKED ==========");
            DBG("REQUEST PROGRAM HEADER");

//            sendProgramHeader(0);
            sysExSender.sendProgramHeader(0);
        };

    DBG(
        "HAS ONCLICK = "
        + juce::String(
            requestRPDATAButton.onClick ? "YES" : "NO"
        )
    );

    requestRPDATAButton.onClick = [this]
        {
            const int programIndex = 0;

            // 1. Program Header
//            sendProgramHeader(programIndex);
            sysExSender.sendProgramHeader(programIndex);

            // 2. Keygroup Header
            juce::Timer::callAfterDelay(
                100,
                [this, programIndex]
                {
//                    sendKGHeader(
//                        programIndex,
//                        0
//                    );
                    sysExSender.sendKGHeader(
                        programIndex,
                        0
                    );
                    
                    
                }
            );

            // 3. KDATA
            juce::Timer::callAfterDelay(
                200,
                [this, programIndex]
                {
//                    sendKData(
//                        programIndex,
//                        0
//                    );
                    sysExSender.sendKData(
                        programIndex,
                        0
                    );
                }
            );

            // 4. Sample Header
            juce::Timer::callAfterDelay(
                400,
                [this]
                {
                    DBG("FORCE RELOAD SAMPLE HEADER ID=1");

                    sendSampleHeader(1);
                }
            );
        };

    DBG("requestRPDATAButton CALLBACK SET");



    addAndMakeVisible(programLabel);
    programLabel.setText("No program", juce::dontSendNotification);



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
        [this](VelocityZone zone)
        {
            DBG("VELOCITY ZONE CHANGED");



            // 選択中のKeygroupが有効か確認
            if (currentKeygroup < 0 ||
                currentKeygroup >= (int)loadedProgram.keygroups.size())
            {
                DBG("INVALID CURRENT KEYGROUP");
                return;
            }

            auto& kg =
                loadedProgram.keygroups[currentKeygroup];

            // 選択中のZoneが有効か確認
            if (currentZone < 0 ||
                currentZone >= (int)kg.zones.size())
            {
                DBG("INVALID CURRENT ZONE");
                return;
            }

            // PC側のProgramデータを更新
            kg.zones[currentZone] = zone;

            DBG("===== LOADED PROGRAM UPDATED =====");

            DBG(
                "KEYGROUP = "
                + juce::String(currentKeygroup)
            );

            DBG(
                "ZONE = "
                + juce::String(currentZone)
            );

            DBG(
                "SAMPLE = ["
                + kg.zones[currentZone].sampleName
                + "]"
            );

            DBG(
                "LOW = "
                + juce::String(
                    kg.zones[currentZone].lowVel
                )
            );

            DBG(
                "HIGH = "
                + juce::String(
                    kg.zones[currentZone].highVel
                )
            );

            programTree.setProgram(
                loadedProgram,
                sampleHeaders
            );

            auto encoded =
                KeygroupEncoder::encode(
                    loadedProgram.keygroups[currentKeygroup]
                );

            DBG(
                "ENCODED LOW = "
                + juce::String(
                    encoded[
                        KeygroupFullOffset::Zone::LOVEL[currentZone]
                    ]
                )
            );

            const auto& raw =
                loadedProgram.keygroups[currentKeygroup].rawData;

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

//            sendKeygroupData(
//                loadedProgram.programNumber,
//                currentKeygroup,
//                encoded
//            );
            
            sysExSender.sendKeygroupData(
                loadedProgram.programNumber,
                currentKeygroup,
                encoded
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

    captureAButton.setBounds(area.removeFromTop(50));
    captureBButton.setBounds(area.removeFromTop(50));
    compareButton.setBounds(area.removeFromTop(50));
    requestButton.setBounds(area.removeFromTop(50));
    requestRPDATAButton.setBounds(area.removeFromTop(50));
    programLabel.setBounds(area.removeFromTop(30));

    // 左側
    auto left = area.removeFromLeft(350);
    programTree.setBounds(left);

    // 右側全部をタブ領域にする
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

void MainComponent::handleIncomingMidiMessage(
    juce::MidiInput*,
    const juce::MidiMessage& message)
{
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

    DBG("=== INCOMING SYSEX ===");
    DBG("opcode = 0x" + juce::String::toHexString(opcode));
    DBG("getSysExDataSize = " + juce::String((int)size));
    DBG("message raw size = " + juce::String(message.getRawDataSize()));

    programBuffer.reset();
    programBuffer.append(data, size);

    static int captureIndex = 0;

    juce::String name =
        (captureIndex == 0)
        ? "rpdata_A"
        : "rpdata_B";

    saveRawRPDATA(name, programBuffer);

    std::vector<uint8_t> decoded;

    switch (opcode)
    {
        case 0x05:
        {
            

            DBG("SLIST RECEIVED");

            DBG("BEFORE parseSLIST");

            parseSLIST(programBuffer);

            DBG("AFTER parseSLIST");

            for (int i = 0; i < programBuffer.getSize(); i++)
            {
                DBG(
                    juce::String(i)
                    + " : 0x"
                    + juce::String::toHexString(programBuffer[i])
                );
            }

            break;
        }

        

        case 0x16:
            handleCommandReply(data, size);
            break;


        case 0x28:
            handleProgramHeaderResponse();
            break;
            
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

            decoded =
                decodeSampleHeader(programBuffer);

            DBG(
                "FULL SAMPLE HEADER SIZE = "
                + juce::String((int)decoded.size())
            );

            if (decoded.size() < 141)
            {
                DBG("INVALID SAMPLE HEADER SIZE");
                break;
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

                break;
            }

            // ==============================
            // 対象Sample IDを取得
            // ==============================

            const int sampleId =
                pendingSampleRequests.front();

            pendingSampleRequests.pop();

            DBG(
                "PENDING SAMPLE ID = "
                + juce::String(sampleId)
            );

            // ==============================
            // Parse
            // ==============================

            SampleHeader sh =
                SampleHeaderParser::parse(
                    decoded,
                    sampleId
                );

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
            // Sample Header情報
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
                            "PROGRAM TREE UPDATED "
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

            break;
        }

            
        case 0x03:
        {
            
            //std::vector<uint8_t> raw(data,data + size);
            //DBG("=== PLIST RAW HEX ===");
            
            //for (int i = 0; i < size; ++i)
            //{
            //    DBG(juce::String(i) + ": " +
            //        juce::String::toHexString((int)data[i]));
            //}
            
            auto rawCopy = std::vector<uint8_t>(data, data + size);
            juce::MessageManager::callAsync([this, rawCopy]()
                                            {
                parsePLIST(rawCopy);
            });
            break;
        }

       

      



        default:
            DBG("Unknown Opcode = 0x" + juce::String::toHexString((int)opcode));
            return;
                    

        }
    captureIndex++;
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
    // Resolve Sample IDs
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

    if (currentKeygroup < 0)
    {
        DBG("INVALID currentKeygroup");
        return;
    }

    if (currentKeygroup >=
        (int)loadedProgram.keygroups.size())
    {
        loadedProgram.keygroups.resize(
            currentKeygroup + 1
        );
    }

    loadedProgram.keygroups[currentKeygroup] = kg;

    DBG(
        "STORED KEYGROUP INDEX = "
        + juce::String(currentKeygroup)
    );

    DBG(
        "AFTER STORE loadedProgram.keygroups.size = "
        + juce::String(
            (int)loadedProgram.keygroups.size()
        )
    );


    // ========================================
    // Request Sample Headers
    // ========================================

    for (const auto& zone :
        loadedProgram.keygroups[currentKeygroup].zones)
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

    juce::MessageManager::callAsync(
        [this]
        {
            DBG("UPDATE TREE FROM KEYGROUP");

            programTree.setProgram(
                loadedProgram,
                sampleHeaders
            );

            DBG(
                "PROGRAM TREE UPDATED "
                "FROM KEYGROUP"
            );
        }
    );

    if (!pendingSampleRequests.empty())
    {
        DBG(
            "WAITING FOR SAMPLE HEADERS. pending="
            + juce::String(
                (int)pendingSampleRequests.size()
            )
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

    uint8_t result = data[4];

    DBG(
        "AKAI REPLY CODE = "
        + juce::String((int)result)
    );
}

    
void MainComponent::handleProgramHeaderResponse()
{
    DBG("========== CASE 0x28 ENTERED ==========");

    DBG(
        "programBuffer SIZE BEFORE DECODE = "
        + juce::String((int)programBuffer.getSize())
    );

    DBG("ABOUT TO decodeProgramHeader");

    auto decoded =
        decodeProgramHeader(programBuffer);

    DBG(
        "PROGRAM SIZE = "
        + juce::String((int)decoded.size())
    );

    auto existingKeygroups =
        std::move(loadedProgram.keygroups);

    loadedProgram =
        ProgramParser::parse(decoded);

    loadedProgram.keygroups =
        std::move(existingKeygroups);

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

    DBG(
        "GROUPS = "
        + juce::String(loadedProgram.groups)
    );

    DBG(
        "KEYGROUPS PRESERVED = "
        + juce::String(
            (int)loadedProgram.keygroups.size()
        )
    );

    for (int i = 0;
         i < loadedProgram.groups;
         ++i)
    {
        sysExSender.sendKGHeader(
            loadedProgram.programNumber,
            i
        );
    }
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


void MainComponent::handleKeygroupHeaderResponse()
{
    DBG("ENTER CASE 0x2A");

    auto decoded =
        decodeKeygroupHeader(programBuffer);

    DBG(
        "decodeKeygroupHeader size = "
        + juce::String((int)decoded.size())
    );

    Keygroup kg =
        KeygroupParser::parse(
            decoded,
            residentSamples
        );

    for (auto& zone : kg.zones)
    {
        if (zone.sampleName.isEmpty())
            continue;

        zone.sampleId =
            findSampleId(
                juce::String(zone.sampleName)
            );

        DBG(
            "Resolved "
            + juce::String(zone.sampleName)
            + " -> "
            + juce::String(zone.sampleId)
        );
    }

    if (currentKeygroup >=
        loadedProgram.keygroups.size())
    {
        loadedProgram.keygroups.resize(
            currentKeygroup + 1
        );
    }

    loadedProgram.keygroups[currentKeygroup] = kg;

    sysExSender.sendRSLIST();

    sysExSender.sendKData(
        loadedProgram.programNumber,
        currentKeygroup
    );

    DBG("ENV1 SUSTAIN = "
        + juce::String(kg.env1.sustain));

    DBG("ENV1 RELEASE = "
        + juce::String(kg.env1.release));
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
    auto* p =
        static_cast<const uint8_t*>(data.getData());

    int count =
        p[4] | (p[5] << 7);

    DBG("SAMPLE COUNT = "
        + juce::String(count));


    residentSamples.clear();


    int offset = 6;

    for (int i = 0; i < count; i++)
    {
        juce::String name;

        for (int j = 0; j < 12; j++)
        {
            name += decodePlistChar(
                p[offset + j]
            );
        }

        name = name.trim();


        // ★ 保存
        residentSamples[i] = name;


        DBG(
            juce::String(i)
            + " : "
            + name
        );


        offset += 12;
    }

    //velocityZoneEditor.setResidentSamples(
    //    residentSamples
    //);


    DBG("RESIDENT SAMPLE SIZE = "
        + juce::String(
            residentSamples.size()
        ));

    const auto samplesCopy =
        residentSamples;

    juce::MessageManager::callAsync(
        [this, samplesCopy]()
        {
            DBG(
                "SETTING RESIDENT SAMPLES TO VELOCITY ZONE EDITOR"
            );

            velocityZoneEditor.setResidentSamples(
                samplesCopy
            );

            DBG(
                "RESIDENT SAMPLES SET TO VELOCITY ZONE EDITOR"
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



void MainComponent::sendSampleHeader(int sampleId)
{
    pendingSampleRequests.push(sampleId);

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

        DBG(juce::String(p) + " : " + name);
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
