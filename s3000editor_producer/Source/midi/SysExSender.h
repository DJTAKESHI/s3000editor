#pragma once

#include <JuceHeader.h>

class SysExSender
{
public:
    SysExSender() = default;

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


    
    
    
private:
    juce::MidiOutput* midiOutput = nullptr;
    
};
