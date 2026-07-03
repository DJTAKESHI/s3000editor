#include "MainComponent.h"
#include <fstream>
#include <juce_gui_basics/juce_gui_basics.h>

//char decodeAkaiChar(uint8_t v)
//{
//    switch (v)
//    {
//    case 0x0A: return ' ';   // space仮
//    case 0x0B: return 'A';
//    case 0x0C: return 'B';
//    case 0x0D: return 'C';
//    case 0x0E: return 'D';
//    case 0x0F: return 'E';
//    case 0x10: return 'F';
//    case 0x11: return 'G';
//    case 0x12: return 'H';
//    case 0x13: return 'I';
//    case 0x14: return 'J';
//    case 0x15: return 'K';
//    case 0x16: return 'L';
//    case 0x17: return 'M';
//    case 0x18: return 'N';
//    case 0x19: return 'O';
//    case 0x1A: return 'P';
//    case 0x1B: return 'Q';
//    case 0x1C: return 'R';
//    case 0x1D: return 'S';
//    case 0x1E: return 'T';
//    case 0x1F: return 'U';
//    default:   return '?';
//    }
//}

//char decodeAkaiChar(uint8_t v)
//{
//    DBG("CHAR RAW = " + juce::String((int)v));
//    return '?';
//}

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
    setSize (600, 400);

    addAndMakeVisible(listBox);
    listBox.setModel(this);

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
            sendRPLIST();
        };

    addAndMakeVisible(requestRPDATAButton);
    requestRPDATAButton.onClick = [this]
        {
            sendRPDATA(0);
        };

    addAndMakeVisible(programLabel);
    programLabel.setText("No program", juce::dontSendNotification);










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

    auto devices = juce::MidiInput::getAvailableDevices();
    

    for (int i = 0; i < devices.size(); ++i)
    {
        DBG(juce::String(i) + ": " + devices[i].name);
    }



    for (auto d : devices)
    {
        DBG("MIDI Device: " + d.name);
    }



    if (!devices.isEmpty())
    {
        midiInput = juce::MidiInput::openDevice(devices[1].identifier, this);

        if (midiInput)
        {
            midiInput->start();
            DBG("MIDI INPUT OPENED");

        }

        else
        {
            DBG("Failed to open midi input");
        }
    }

    auto outputs = juce::MidiOutput::getAvailableDevices();

    for (auto& d : outputs)
    {
        DBG("MIDI OUT: " + d.name);
    }

    if (!outputs.isEmpty())
    {
        midiOutput = juce::MidiOutput::openDevice(
            outputs[2].identifier
        );

        if (midiOutput)
        {
            DBG("MIDI OUTPUT OPENED");
        }
    }

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
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    auto area = getLocalBounds();

    captureAButton.setBounds(area.removeFromTop(50));
    captureBButton.setBounds(area.removeFromTop(50));
    compareButton.setBounds(area.removeFromTop(50));
    requestButton.setBounds(area.removeFromTop(50));
    requestRPDATAButton.setBounds(area.removeFromTop(50));
    programLabel.setBounds(area.removeFromTop(30));
    listBox.setBounds(area);

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

void MainComponent::handleIncomingMidiMessage(juce::MidiInput*, const juce::MidiMessage& message)
{
    if (!message.isSysEx())
        return;

    auto* data = message.getSysExData();
    auto size = message.getSysExDataSize();

    if (size < 4)
    {
        return;
    }

    if (data[0] != 0x47 || data[1] != 0x00)
    {
        return;
    }

    uint8_t opcode = data[2];
        
    programBuffer.reset();
    programBuffer.append(data, size);

    static int captureIndex = 0;
    juce::String name = (captureIndex == 0) ? "rpdata_A" : "rpdata_B";

    saveRawRPDATA(name, programBuffer);
    
    
    std::vector<uint8_t> decoded = decodeNibbleData(programBuffer);

    switch (opcode)
    {
    case 0x28:
        DBG("PROGRAM HEADER RECEIVED");
        //parseProgram(decoded);
        break;

    case 0x2A:
        DBG("KEYGROUP HEADER RECEIVED");
        //parseKeygroup(decoded);
        break;

    case 0x07:
        DBG("RPDATA RESPONSE RECEIVED");
        parseRPDATA(decoded);
        break;
        
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

        //std::vector<uint8_t> raw;
        //raw.assign(data, data + size);
        //DBG("PLIST RECEIVED");
        //parsePLIST(raw);
        //break;

    }

    //case 0x03:
    //{
    //    DBG("PLIST RECEIVED");

    //    DBG("decoded size = " + juce::String((int)decoded.size()));

    //    for (int i = 0; i < 32 && i < decoded.size(); ++i)
    //        DBG(juce::String(i) + " : "
    //            + juce::String::toHexString(decoded[i]));

    //    parsePLIST(decoded);

    //    break;
    //}

    default:
        DBG("Unknown Opcode = 0x" + juce::String::toHexString((int)opcode));
        return;
                

    }

    captureIndex++;


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

void MainComponent::sendRPLIST()
{
    if (!midiOutput)
    {
        return;
    }

    uint8_t data[]
    {
        0x47,
        0x00,
        0x02,
        0x48
    };

    auto msg = juce::MidiMessage::createSysExMessage(
        data,
        sizeof(data));
    midiOutput->sendMessageNow(msg);

    DBG("RPLIST SENT");




}

void MainComponent::sendRPDATA(int programIndex)
{
    if (!midiOutput)
    {
        return;
    }

    uint8_t data[]
    {
        0x47,
        0x00,
        0x06,
        0x48,
        (uint8_t)(programIndex & 0x7F),
        (uint8_t)((programIndex >> 7) & 0x7F)
    };

    auto msg = juce::MidiMessage::createSysExMessage(
        data,
        sizeof(data));

    midiOutput->sendMessageNow(msg);

    DBG("RPDATA SENT");
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

    int panOffset = 0x50;
    int freqOffset = 0x51;
    int filterOffset = 0x52;

    auto params = parseParams(decoded);


    DBG("DECODED SIZE = "
        + juce::String((int)decoded.size()));
    //auto params = parseParams(decoded);

    DBG("PAN = " + juce::String(params.pan));
    DBG("FREQ = " + juce::String(params.frequency));
    DBG("FILTER = " + juce::String(params.filter));
}

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


//std::vector<uint8_t> MainComponent::decodeNibbleData(const juce::MemoryBlock& data)
//{
//    std::vector<uint8_t> decoded;
//
//    const uint8_t* p = (const uint8_t*)(data.getData());
//
//    size_t size = data.getSize();
//    const int headerSize = 4;
//
//    DBG("RAW HEX:");
//    for (int i = 0; i < 32; ++i)
//        DBG(juce::String::toHexString(((uint8_t*)data.getData())[i]));
//
//    for (size_t i = headerSize; i + 1 < size; i += 2)
//    {
//        uint8_t low = p[i];
//        uint8_t high = p[i + 1];
//
//        if (low > 0x0F || high > 0x0F)
//        {
//            DBG("INVALID NIBBLE at " + juce::String(i));
//        }
//
//
//        uint8_t value = (high << 4) | low;
//
//        decoded.push_back(value);
//
//    }
//
//    DBG("decoded size = " + juce::String((int)decoded.size()));
//
//    return decoded;
//
//}

std::vector<uint8_t> MainComponent::decodeNibbleData(const juce::MemoryBlock& data)
{
    std::vector<uint8_t> decoded;

    const uint8_t* p = (const uint8_t*)data.getData();
    size_t size = data.getSize();

    for (size_t i = 4; i < size; ++i)
    {
        decoded.push_back(p[i] & 0x7F);
    }

    DBG("decoded size = " + juce::String((int)decoded.size()));
    return decoded;
}


std::string decodeName(const uint8_t* p)
{
    std::string s;
    for (int i = 0; i < 12; i++)
    {
        s += decodeAkaiChar(p[i + 4]);
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

    ProgramParams params = parseParams(
        decoded
        //0x50,
        //0x51,
        //0x52
    );
    DBG("===== PROGRAM =====");
    DBG("SIZE   = " + juce::String((int)decoded.size()));
    DBG("PAN    = " + juce::String(params.pan));
    DBG("FREQ   = " + juce::String(params.frequency));
    DBG("FILTER = " + juce::String(params.filter));


}

void MainComponent::parseKeygroup(
    const std::vector<uint8_t>& decoded
)
{
    DBG("====KEYGROUP=====");
    DBG("Size = " + juce::String((int)decoded.size()));

    static int dumpIndex = 0;

    juce::File file =
        juce::File::getSpecialLocation(
            juce::File::userDesktopDirectory
        ).getChildFile(
            "keygroup_" + juce::String(dumpIndex++) + ".bin");

    file.replaceWithData(
        decoded.data(),
        decoded.size()
    );
}

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

//ProgramParams MainComponent::parseParams(const std::vector<uint8_t>& d,
//    int panOffset,
//    int freqOffset,
//    int filterOffset)
//
//{
//    ProgramParams p{};
//
//    
//
//    if (freqOffset >= (int)d.size())
//    {
//        DBG("parseParams OUT OF RANGE");
//        return p;
//    }
//
//    p.pan = (int8_t)d[panOffset];
//    p.frequency = d[freqOffset];
//    p.filter = d[filterOffset];
//
//    return p;
//}

ProgramParams MainComponent::parseParams(const std::vector<uint8_t>& d)
{
    ProgramParams p{};

    if (d.size() < 3)
        return p;

    p.pan = (int8_t)d[0];
    p.frequency = d[1];
    p.filter = d[2];

    return p;
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

void MainComponent::listBoxItemClicked(int row, const juce::MouseEvent&)
{
    if (row < 0 || row >= programList.size())
    {
        return;
    }

    sendRPDATA(programList[row].index);
}

//void MainComponent::saveRawSysEx(const uint8_t* data, size_t size)
//{
//    auto file = juce::File::getSpecialLocation(
//        juce::File::userDesktopDirectory);
//
//    file = file.getChildFile(isCaptureA ? "rpdata_raw_A.bin"
//        : "rpdata_raw_B.bin");
//
//    file.appendData(data, size);
//
//    DBG("Saved RAW SysEx: " + file.getFullPathName());
//}
