#include "SysExSender.h"
#include "../s3000/Offsets.h"


void SysExSender::setMidiOutput(juce::MidiOutput* output)
{
    midiOutput = output;
}

void SysExSender::sendRPLIST()
{
    if (midiOutput == nullptr)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    uint8_t data[]
    {
        0x47,
        0x00,
        0x02,
        0x48
    };

    auto msg =
        juce::MidiMessage::createSysExMessage(
            data,
            sizeof(data)
        );

    midiOutput->sendMessageNow(msg);

    DBG("RPLIST SENT");
}

void SysExSender::sendProgramData(
    int programIndex,
    const std::vector<uint8_t>& data)
{
    if (!midiOutput)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    std::vector<uint8_t> sysex;

    sysex.push_back(0x47);
    sysex.push_back(0x00); // exclusive channel
    sysex.push_back(0x07); // PDATA
    sysex.push_back(0x48);

    // Program number
    sysex.push_back(
        static_cast<uint8_t>(
            programIndex & 0x7F
            )
    );

    sysex.push_back(
        static_cast<uint8_t>(
            (programIndex >> 7) & 0x7F
            )
    );

    DBG(
        "PDATA RAW STEREO OFFSET 23 = "
        + juce::String(
            (int)data[
                ProgramOffset::Output::Stereo
            ]
        )
    );

    DBG(
        "PDATA STEREO NIBBLES LOW="
        + juce::String(
            (int)(data[23] & 0x0F)
        )
        + " HIGH="
        + juce::String(
            (int)((data[23] >> 4) & 0x0F)
        )
    );


    // Program Common Data
    // 1 byte -> low nibble / high nibble
    for (const auto byte : data)
    {
        sysex.push_back(
            static_cast<uint8_t>(
                byte & 0x0F
                )
        );

        sysex.push_back(
            static_cast<uint8_t>(
                (byte >> 4) & 0x0F
                )
        );
    }

    auto msg =
        juce::MidiMessage::createSysExMessage(
            sysex.data(),
            static_cast<int>(sysex.size())
        );

    DBG(
        "SEND PDATA PROGRAM="
        + juce::String(programIndex)
        + " RAW SIZE="
        + juce::String((int)data.size())
        + " SYSEX SIZE="
        + juce::String((int)sysex.size())
    );

    midiOutput->sendMessageNow(msg);

    DBG("PDATA SENT");
}


void SysExSender::sendProgramHeader(int programIndex)
{
    DBG("========== sendProgramHeader CALLED ==========");

    DBG(
        "PROGRAM INDEX = "
        + juce::String(programIndex)
    );

    if (!midiOutput)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    DBG("MIDI OUTPUT OK");

    uint8_t data[]
    {
        0x47,
        0x00,
        0x27,
        0x48,

        static_cast<uint8_t>(
            programIndex & 0x7F
        ),

        static_cast<uint8_t>(
            (programIndex >> 7) & 0x7F
        ),

        0x00, // Reserved

        0x00,
        0x00, // offset = 0

        0x40,
        0x01  // length = 192
    };

    DBG("REQUEST DATA BUILT");

    auto msg =
        juce::MidiMessage::createSysExMessage(
            data,
            sizeof(data)
        );

    DBG("SYSEX MESSAGE CREATED");

    midiOutput->sendMessageNow(msg);

    DBG("PROGRAM HEADER REQUEST SENT");
}

void SysExSender::sendDeleteKeygroup(
    int programIndex,
    int keygroupIndex)
{
    if (!midiOutput)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    std::vector<uint8_t> sysex;

    // Akai header
    sysex.push_back(0x47);
    sysex.push_back(0x00);
    sysex.push_back(0x13); // DELK
    sysex.push_back(0x48);

    // Program number
    sysex.push_back(
        static_cast<uint8_t>(
            programIndex & 0x7F
            )
    );

    sysex.push_back(
        static_cast<uint8_t>(
            (programIndex >> 7) & 0x7F
            )
    );

    // Keygroup number
    sysex.push_back(
        static_cast<uint8_t>(
            keygroupIndex & 0x7F
            )
    );

    DBG(
        "DELETE KEYGROUP PROGRAM="
        + juce::String(programIndex)
        + " KG="
        + juce::String(keygroupIndex)
    );

    auto message =
        juce::MidiMessage::createSysExMessage(
            sysex.data(),
            sysex.size()
        );

    midiOutput->sendMessageNow(
        message
    );

    DBG("DELETE KEYGROUP SENT");
}


void SysExSender::sendKGHeader(
    int programIndex,
    int keygroup)
{
    DBG("sendKGHeader called");
    DBG("program = " + juce::String(programIndex));
    DBG("keygroup = " + juce::String(keygroup));

    if (!midiOutput)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    uint8_t data[]
    {
        0x47, // Akai
        0x00, // MIDI exclusive channel
        0x29, // Request Keygroup Header bytes
        0x48, // S1000 identity

        static_cast<uint8_t>(programIndex & 0x7F),
        static_cast<uint8_t>((programIndex >> 7) & 0x7F),
        static_cast<uint8_t>(keygroup & 0x7F),

        // offset
        0x00,
        0x00,

        // number of bytes
        0x40,
        0x00
    };

    auto msg =
        juce::MidiMessage::createSysExMessage(
            data,
            sizeof(data)
        );

    midiOutput->sendMessageNow(msg);

    DBG("KG HEADER SENT");
}

void SysExSender::sendKData(
    int programIndex,
    int keygroup)
{
    DBG("sendKData called");

    if (!midiOutput)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    uint8_t data[]
    {
        0x47, // Akai
        0x00, // channel
        0x08, // RKDATA
        0x48, // S1000

        // program number
        static_cast<uint8_t>(programIndex & 0x7F),
        static_cast<uint8_t>((programIndex >> 7) & 0x7F),

        // keygroup
        static_cast<uint8_t>(keygroup & 0x7F)
    };

    auto msg =
        juce::MidiMessage::createSysExMessage(
            data,
            sizeof(data)
        );

    midiOutput->sendMessageNow(msg);

    DBG("RKDATA SENT");
}

void SysExSender::sendSampleHeader(int sampleId)
{
    constexpr int headerLength = SampleHeaderFormat::Size;

    DBG("sendSampleHeader called");
    DBG("sampleId = "
        + juce::String(sampleId));
    DBG("LOW BYTE = "
        + juce::String(sampleId & 0x7F));
    DBG("HIGH BYTE = "
        + juce::String((sampleId >> 7) & 0x7F));

    if (!midiOutput)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    uint8_t data[]
    {
        0x47, // Akai
        0x00, // MIDI channel
        0x2B, // Request Sample Header
        0x48, // S1000 identity

        static_cast<uint8_t>(sampleId & 0x7F),
        static_cast<uint8_t>((sampleId >> 7) & 0x7F),

        0x00, // reserved

        0x00, // offset
        0x00,

        static_cast<uint8_t>(headerLength & 0x7F),
        static_cast<uint8_t>((headerLength >> 7) & 0x7F)
    };

    auto msg =
        juce::MidiMessage::createSysExMessage(
            data,
            sizeof(data)
        );

    midiOutput->sendMessageNow(msg);

    DBG("SAMPLE HEADER REQUEST SENT");
}

void SysExSender::sendKeygroupData(
    int programIndex,
    int keygroupIndex,
    const std::vector<uint8_t>& data)
{
    if (!midiOutput)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    std::vector<uint8_t> sysex;

    // Akai header
    sysex.push_back(0x47);
    sysex.push_back(0x00);
    sysex.push_back(0x09);
    sysex.push_back(0x48);

    // Program number (14 bit)
    sysex.push_back(
        static_cast<uint8_t>(
            programIndex & 0x7F
        )
    );

    sysex.push_back(
        static_cast<uint8_t>(
            (programIndex >> 7) & 0x7F
        )
    );

    // Keygroup number
    sysex.push_back(
        static_cast<uint8_t>(
            keygroupIndex & 0x7F
        )
    );

    // Nibble encoding
    for (uint8_t value : data)
    {
        sysex.push_back(
            value & 0x0F
        );

        sysex.push_back(
            (value >> 4) & 0x0F
        );
    }

    DBG(
        "KEYGROUP SYSEX SIZE = "
        + juce::String((int)sysex.size())
    );

    auto message =
        juce::MidiMessage::createSysExMessage(
            sysex.data(),
            sysex.size()
        );

    midiOutput->sendMessageNow(message);

    DBG("KEYGROUP DATA SENT");
}

void SysExSender::sendSysEx(
    const std::vector<uint8_t>& data)
{
    if (!midiOutput)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    if (data.empty())
    {
        DBG("EMPTY SYSEX DATA");
        return;
    }

    auto msg =
        juce::MidiMessage::createSysExMessage(
            data.data(),
            data.size()
        );

    midiOutput->sendMessageNow(msg);

    DBG("SYSEX SENT");
}

void SysExSender::sendRSLIST()
{
    if (!midiOutput)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    uint8_t data[]
    {
        0x47,
        0x00,
        0x04,
        0x48
    };

    auto msg =
        juce::MidiMessage::createSysExMessage(
            data,
            sizeof(data)
        );

    midiOutput->sendMessageNow(msg);

    DBG("RSLIST REQUEST SENT");
}

void SysExSender::sendHeartbeat()
{
    if (!midiOutput)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    uint8_t data[]
    {
        0x47,   // Akai Manufacturer
        0x00,   // Exclusive channel
        0x10,   // RMDATA
        0x48    // Model identity
    };

    auto msg =
        juce::MidiMessage::createSysExMessage(
            data,
            sizeof(data)
        );

    midiOutput->sendMessageNow(msg);

    DBG("HEARTBEAT RMDATA SENT");
}
