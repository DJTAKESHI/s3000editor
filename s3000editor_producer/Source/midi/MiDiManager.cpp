
#include "MidiManager.h"

bool MidiManager::openOutput(int deviceIndex)
{
    auto outputs = juce::MidiOutput::getAvailableDevices();

    if (deviceIndex < 0 ||
        deviceIndex >= outputs.size())
    {
        DBG("INVALID MIDI OUTPUT INDEX");
        return false;
    }

    midiOutput =
        juce::MidiOutput::openDevice(
            outputs[deviceIndex].identifier
        );

    if (!midiOutput)
    {
        DBG("FAILED TO OPEN MIDI OUTPUT");
        return false;
    }

    DBG(
        "MIDI OUTPUT OPENED: "
        + outputs[deviceIndex].name
    );

    return true;
}

juce::MidiOutput* MidiManager::getOutput()
{
    return midiOutput.get();
}

bool MidiManager::openInput(
    int deviceIndex,
    juce::MidiInputCallback* callback)
{
    auto inputs =
        juce::MidiInput::getAvailableDevices();

    if (deviceIndex < 0 ||
        deviceIndex >= inputs.size())
    {
        DBG("INVALID MIDI INPUT INDEX");
        return false;
    }

    midiInput =
        juce::MidiInput::openDevice(
            inputs[deviceIndex].identifier,
            callback
        );

    if (!midiInput)
    {
        DBG("FAILED TO OPEN MIDI INPUT");
        return false;
    }

    midiInput->start();

    DBG(
        "MIDI INPUT OPENED: "
        + inputs[deviceIndex].name
    );

    return true;
}
