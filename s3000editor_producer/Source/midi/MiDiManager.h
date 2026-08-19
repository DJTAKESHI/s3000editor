#pragma once

#include <JuceHeader.h>

class MidiManager
{
public:
    bool openOutput(int deviceIndex);

    bool openInput(
        int deviceIndex,
        juce::MidiInputCallback* callback
    );

    juce::MidiOutput* getOutput();

private:
    std::unique_ptr<juce::MidiInput> midiInput;
    std::unique_ptr<juce::MidiOutput> midiOutput;
};
