#include "SysExSender.h"
#include "../s3000/Offsets.h"


void SysExSender::setMidiOutput(juce::MidiOutput* output)
{
    midiOutput = output;
}

void SysExSender::sendRPLIST() // request list of resident program names
{
    if (midiOutput == nullptr)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    uint8_t data[]
    {
        0x47, //Akai Manufacturer code
        0x00, // exclusive channel
        0x02, //Function code of RPlist
        0x48 //S1000 Model identity
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


    midiOutput->sendMessageNow(msg);

    DBG("PDATA SENT");
}


void SysExSender::sendProgramHeader(int programIndex)
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
        0x27, //Request for Program Header bytes
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


    auto msg =
        juce::MidiMessage::createSysExMessage(
            data,
            sizeof(data)
        );



    midiOutput->sendMessageNow(msg);


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
    sysex.push_back(0x13); // The function of deleting Keygroup
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

    if (!midiOutput)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    uint8_t data[]
    {
        0x47, 
        0x00, 
        0x29, // Request Keygroup Header bytes
        0x48, 

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

}

void SysExSender::sendKeygroupHeaderByte(
    int programIndex,
    int keygroup,
    int offset,
    int value)
{
    if (!midiOutput)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    const uint8_t raw =
        static_cast<uint8_t>(
            static_cast<int8_t>(value)
            );

    uint8_t data[]
    {
        0x47,
        0x00,
        0x2A,
        0x48,

        static_cast<uint8_t>(programIndex & 0x7F),
        static_cast<uint8_t>((programIndex >> 7) & 0x7F),

        static_cast<uint8_t>(keygroup & 0x7F),

        static_cast<uint8_t>(offset & 0x7F),
        static_cast<uint8_t>((offset >> 7) & 0x7F),

        0x01,
        0x00,

        static_cast<uint8_t>(raw & 0x0F),
        static_cast<uint8_t>((raw >> 4) & 0x0F)
    };

    auto msg =
        juce::MidiMessage::createSysExMessage(
            data,
            sizeof(data)
        );

    midiOutput->sendMessageNow(msg);

    DBG(
        "SEND KG HEADER BYTE KG="
        + juce::String(keygroup)
        + " OFFSET="
        + juce::String(offset)
        + " VALUE="
        + juce::String(value)
    );
}

void SysExSender::sendProgramHeaderByte(
    int programIndex,
    int offset,
    int value)
{
    if (!midiOutput)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    const uint8_t raw =
        static_cast<uint8_t>(
            static_cast<int8_t>(value)
            );

    uint8_t data[]
    {
        0x47,
        0x00,
        0x28,
        0x48,

        static_cast<uint8_t>(programIndex & 0x7F),
        static_cast<uint8_t>((programIndex >> 7) & 0x7F),

        0x00, // Reserved 

        static_cast<uint8_t>(offset & 0x7F),
        static_cast<uint8_t>((offset >> 7) & 0x7F),

        0x01,
        0x00,

        static_cast<uint8_t>(raw & 0x0F),
        static_cast<uint8_t>((raw >> 4) & 0x0F)
    };

    auto msg =
        juce::MidiMessage::createSysExMessage(
            data,
            sizeof(data)
        );

    midiOutput->sendMessageNow(msg);

}

void SysExSender::sendMidiMessage(
    const juce::MidiMessage& message)
{
    if (!midiOutput)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    midiOutput->sendMessageNow(
        message
    );
}


void SysExSender::sendKeygroupHeaderByteRequest(
    int programIndex,
    int keygroup,
    int offset)
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
        0x29,
        0x48,

        static_cast<uint8_t>(programIndex & 0x7F),
        static_cast<uint8_t>((programIndex >> 7) & 0x7F),

        static_cast<uint8_t>(keygroup & 0x7F),

        static_cast<uint8_t>(offset & 0x7F),
        static_cast<uint8_t>((offset >> 7) & 0x7F),

        0x01,
        0x00
    };

    auto msg =
        juce::MidiMessage::createSysExMessage(
            data,
            sizeof(data)
        );

    midiOutput->sendMessageNow(msg);


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


    auto message =
        juce::MidiMessage::createSysExMessage(
            sysex.data(),
            sysex.size()
        );


    midiOutput->sendMessageNow(message);


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

 
}


// Request miscellaneout data (RMDATA) from the S3000XL.
// The reply (MData, function code 0x11) is used as a heartbeat
// to monitor the sampler's curretnly selected program
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
        0x10,   // RMDATA: Request Miscellaneous Data
        0x48    // Model identity
    };

    auto msg =
        juce::MidiMessage::createSysExMessage(
            data,
            sizeof(data)
        );

    midiOutput->sendMessageNow(msg);


}



void SysExSender::sendNoteOn(
    int noteNumber,
    int velocity,
    int midiChannel)
{

    if (midiOutput == nullptr)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    noteNumber = juce::jlimit(0, 127, noteNumber);
    velocity = juce::jlimit(1, 127, velocity);
    midiChannel = juce::jlimit(1, 16, midiChannel);

    auto message =
        juce::MidiMessage::noteOn(
            midiChannel,
            noteNumber,
            (juce::uint8)velocity
        );


    midiOutput->sendMessageNow(message);

    DBG("NOTE ON SENT");
}

void SysExSender::sendNoteOff(
    int noteNumber,
    int midiChannel)
{
    if (midiOutput == nullptr)
        return;

    noteNumber = juce::jlimit(0, 127, noteNumber);
    midiChannel = juce::jlimit(1, 16, midiChannel);

    auto message =
        juce::MidiMessage::noteOff(
            midiChannel,
            noteNumber
        );

    midiOutput->sendMessageNow(message);
}

void SysExSender::sendProgramChange(
    int programNumber,
    int midiChannel)
{
    if (midiOutput == nullptr)
        return;

    programNumber =
        juce::jlimit(0, 127, programNumber);

    midiChannel =
        juce::jlimit(1, 16, midiChannel);

    auto message =
        juce::MidiMessage::programChange(
            midiChannel,
            programNumber
        );

    midiOutput->sendMessageNow(message);
}

void SysExSender::sendDeleteProgram(
    int programNumber)
{
    if (midiOutput == nullptr)
        return;

    programNumber =
        juce::jlimit(0, 16383, programNumber);

    uint8_t data[]
    {
        0x47,
        0x00, // Exclusive channel
        0x12, // DELP
        0x48,

        static_cast<uint8_t>(
            programNumber & 0x7F),

        static_cast<uint8_t>(
            (programNumber >> 7) & 0x7F)
    };

    auto message =
        juce::MidiMessage::createSysExMessage(
            data,
            sizeof(data)
        );

    midiOutput->sendMessageNow(message);
}

// Partial write to a keygroup header
// Writes a single byte at the specified offset instead of
// sending the entire Keygroup Header.
void SysExSender::sendKeygroupByte(
    int programIndex,
    int keygroupIndex,
    int byteOffset,
    uint8_t value
)
{
    if (!midiOutput)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    std::vector<uint8_t> sysex;

    // Akai header
    sysex.push_back(0x47); // Manufacturer
    sysex.push_back(0x00); // Exclusive channel
    sysex.push_back(0x2A); // Keygroup Header Data
    sysex.push_back(0x48); // Model identity

    // Program number: 14-bit
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

    // Byte offset: 14-bit
    sysex.push_back(
        static_cast<uint8_t>(
            byteOffset & 0x7F
            )
    );

    sysex.push_back(
        static_cast<uint8_t>(
            (byteOffset >> 7) & 0x7F
            )
    );

    // Data length = 1 byte
    sysex.push_back(0x01);
    sysex.push_back(0x00);

    // Nibble-encoded value
    sysex.push_back(
        value & 0x0F
    );

    sysex.push_back(
        (value >> 4) & 0x0F
    );

    const auto message =
        juce::MidiMessage::createSysExMessage(
            sysex.data(),
            sysex.size()
        );

    midiOutput->sendMessageNow(message);
}


// Partial write to a Keygroup Header.
// Writes a 16-bit (2-byte) value starting at the specified offset.
void SysExSender::sendKeygroupWord(
    int programIndex,
    int keygroupIndex,
    int byteOffset,
    uint16_t value
)
{
    if (!midiOutput)
    {
        DBG("NO MIDI OUTPUT");
        return;
    }

    std::vector<uint8_t> sysex;

    sysex.push_back(0x47);
    sysex.push_back(0x00);
    sysex.push_back(0x2A);
    sysex.push_back(0x48);

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

    sysex.push_back(
        static_cast<uint8_t>(
            keygroupIndex & 0x7F
            )
    );

    sysex.push_back(
        static_cast<uint8_t>(
            byteOffset & 0x7F
            )
    );

    sysex.push_back(
        static_cast<uint8_t>(
            (byteOffset >> 7) & 0x7F
            )
    );

    // Data length = 2 bytes
    sysex.push_back(0x02);
    sysex.push_back(0x00);

    const uint8_t lowByte =
        static_cast<uint8_t>(value & 0xFF);

    const uint8_t highByte =
        static_cast<uint8_t>((value >> 8) & 0xFF);

    sysex.push_back(lowByte & 0x0F);
    sysex.push_back((lowByte >> 4) & 0x0F);

    sysex.push_back(highByte & 0x0F);
    sysex.push_back((highByte >> 4) & 0x0F);

    const auto message =
        juce::MidiMessage::createSysExMessage(
            sysex.data(),
            sysex.size()
        );

    midiOutput->sendMessageNow(message);
}