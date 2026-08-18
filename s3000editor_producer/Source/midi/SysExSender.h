#pragma once

#include <JuceHeader.h>

class SysExSender
{
public:
    SysExSender() = default;

    void setMidiOutput(juce::MidiOutput* output);

    void sendRPLIST();
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
    
    
    
private:
    juce::MidiOutput* midiOutput = nullptr;
    
};
