#pragma once

#include <JuceHeader.h>
#include "../s3000/S3000Types.h"
#include "ProgramRangeBar.h"

class ProgramEditor : public juce::Component
{
public:
    ProgramEditor();

    void setProgram(const Program& program);
    void resized() override;
    void paint(juce::Graphics& g) override;
    std::function<void(const Program&)>
        onProgramChanged;


private:
    Program currentProgram;

    juce::Label titleLabel;

    juce::Label generalSectionLabel;
    juce::Label outputSectionLabel;


    //juce::Label midiChannelLabel;
    juce::ComboBox midiChannelCombo;


    juce::Label midiChannelLabel;
    juce::TextEditor midiChannelEditor;

    juce::Label polyphonyLabel;
    juce::ComboBox polyphonyCombo;

    juce::Label priorityLabel;
    juce::ComboBox priorityCombo;

    juce::Label playLowLabel;
    juce::TextEditor playLowEditor;

    juce::Label playHighLabel;
    juce::TextEditor playHighEditor;

    ProgramRangeBar playRangeBar;

    juce::Label outputLabel;
    juce::ComboBox outputCombo;

    juce::Label stereoLabel;
    juce::Slider stereoSlider;
    juce::Label stereoValueLabel;

    juce::Label panLabel;
    juce::Slider panSlider;
    juce::Label panValueLabel;

    juce::Label loudnessLabel;
    juce::Slider loudnessSlider;
    juce::Label loudnessValueLabel;

    juce::Label tuneLabel;
    juce::Slider tuneSlider;
    juce::Label tuneValueLabel;

    juce::Label outLevelLabel;
    juce::Slider outLevelSlider;
    juce::Label outLevelValueLabel;

    juce::Label pitchVoiceSectionLabel;
    juce::Rectangle<int> pitchVoiceCardBounds;

    juce::Label bendUpLabel;
    juce::Slider bendUpSlider;
    juce::Label bendUpValueLabel;

    juce::Label pressurePitchLabel;
    juce::Slider pressurePitchSlider;
    juce::Label pressurePitchValueLabel;

    juce::Label bendDownLabel;
    juce::Slider bendDownSlider;
    juce::Label bendDownValueLabel;

    juce::Label bendModeLabel;
    juce::ComboBox bendModeCombo;

    juce::Label transposeLabel;
    juce::Slider transposeSlider;
    juce::Label transposeValueLabel;

    juce::Label legatoLabel;
    juce::ComboBox legatoCombo;

    juce::Label voiceAssignLabel;
    juce::ComboBox voiceAssignCombo;

    juce::Label lfo1DesyncLabel;
    juce::ComboBox lfo1DesyncCombo;

    juce::Label lfo1SectionLabel;
    juce::Rectangle<int> lfo1CardBounds;

    juce::Label lfo1RateLabel;
    juce::Slider lfo1RateSlider;
    juce::Label lfo1RateValueLabel;




    juce::Rectangle<int> generalCardBounds;
    juce::Rectangle<int> outputCardBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ProgramEditor
    )
};