#pragma once

#include <JuceHeader.h>
#include "../s3000/S3000Types.h"

class ProgramEditor : public juce::Component
{
public:
    ProgramEditor();

    void setProgram(const Program& program);
    void resized() override;

private:
    Program currentProgram;

    juce::Label titleLabel;

    juce::Label midiChannelLabel;
    juce::TextEditor midiChannelEditor;

    juce::Label polyphonyLabel;
    juce::TextEditor polyphonyEditor;

    juce::Label priorityLabel;
    juce::TextEditor priorityEditor;

    juce::Label playLowLabel;
    juce::TextEditor playLowEditor;

    juce::Label playHighLabel;
    juce::TextEditor playHighEditor;

    juce::Label outputLabel;
    juce::TextEditor outputEditor;

    juce::Label stereoLabel;
    juce::TextEditor stereoEditor;

    juce::Label panLabel;
    juce::TextEditor panEditor;

    juce::Label loudnessLabel;
    juce::TextEditor loudnessEditor;

    juce::Label tuneLabel;
    juce::TextEditor tuneEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ProgramEditor
    )
};