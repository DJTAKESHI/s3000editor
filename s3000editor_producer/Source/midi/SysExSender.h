#pragma once

#include <JuceHeader.h>

class SysExSender
{
public:
    SysExSender() = default;

    void sendKeygroupByte(
        int programIndex,
        int keygroupIndex,
        int byteOffset,
        uint8_t value
    );

    void sendProgramHeaderByte(
        int programIndex,
        int offset,
        int value);


    void sendKeygroupHeaderByte(
        int programIndex,
        int keygroup,
        int offset,
        int value);

    void setMidiOutput(juce::MidiOutput* output);

    void sendRPLIST();
    void sendRSLIST();
    void sendHeartbeat();

    void sendNoteOn(
        int noteNumber,
        int velocity,
        int midiChannel = 1);

    void sendNoteOff(
        int noteNumber,
        int midiChannel = 1);

    
    void sendProgramHeader(int programIndex);
    void sendKGHeader(
        int programIndex,
        int keygroup);
    void sendKeygroupHeaderByteRequest(
        int programIndex,
        int keygroup,
        int offset);

    
    void sendKData(
        int programIndex,
        int keygroup);
    
    void sendSampleHeader(int sampleId);
    
    void sendKeygroupData(
        int programIndex,
        int keygroupIndex,
        const std::vector<uint8_t>& data);
    
    void sendSysEx(const std::vector<uint8_t>& data);

    void sendProgramData(
        int programIndex,
        const std::vector<uint8_t>& data);

    void sendDeleteKeygroup(
        int programIndex,
        int keygroupIndex);

    void sendProgramChange(
        int programNumber,
        int midiChannel = 16);

    void sendDeleteProgram(int programNumber);

    void sendKeygroupWord(
        int programIndex,
        int keygroupIndex,
        int byteOffset,
        uint16_t value
    );

    void sendMidiMessage(
        const juce::MidiMessage& message
    );



    
    
    
private:
    juce::MidiOutput* midiOutput = nullptr;
    
};
