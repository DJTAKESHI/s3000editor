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

    std::function<void(int)> onEnv2PitchChanged;



private:
    Program currentProgram;

    juce::Label titleLabel;

    juce::Label generalSectionLabel;

    juce::Label outputSectionLabel;

    juce::Label programNameLabel;
    juce::TextEditor programNameEditor;




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

    juce::Label lfo1DepthLabel;
    juce::Slider lfo1DepthSlider;
    juce::Label lfo1DepthValueLabel;

    juce::Label lfo1DelayLabel;
    juce::Slider lfo1DelaySlider;
    juce::Label lfo1DelayValueLabel;

    juce::Label lfo1WaveLabel;
    juce::ComboBox lfo1WaveCombo;

    juce::Label lfo2SectionLabel;
    juce::Rectangle<int> lfo2CardBounds;

    juce::Label lfo2RateLabel;
    juce::Slider lfo2RateSlider;
    juce::Label lfo2RateValueLabel;

    juce::Label lfo2DepthLabel;
    juce::Slider lfo2DepthSlider;
    juce::Label lfo2DepthValueLabel;

    juce::Label lfo2DelayLabel;
    juce::Slider lfo2DelaySlider;
    juce::Label lfo2DelayValueLabel;

    juce::Label lfo2WaveLabel;
    juce::ComboBox lfo2WaveCombo;

    juce::Label softPedalSectionLabel;
    juce::Rectangle<int> softPedalCardBounds;

    juce::Label softLoudnessLabel;
    juce::Slider softLoudnessSlider;
    juce::Label softLoudnessValueLabel;

    juce::Label softAttackLabel;
    juce::Slider softAttackSlider;
    juce::Label softAttackValueLabel;

    juce::Label softFilterLabel;
    juce::Slider softFilterSlider;
    juce::Label softFilterValueLabel;

    // =========================
// MODULATION - PAN
// =========================

    juce::Label modPanSectionLabel;
    juce::Rectangle<int> modPanCardBounds;

    juce::Label modPan1Label;
    juce::ComboBox modPan1SourceCombo;
    juce::Slider modPan1AmountSlider;
    juce::Label modPan1AmountLabel;

    juce::Label modPan2Label;
    juce::ComboBox modPan2SourceCombo;
    juce::Slider modPan2AmountSlider;
    juce::Label modPan2AmountLabel;

    juce::Label modPan3Label;
    juce::ComboBox modPan3SourceCombo;
    juce::Slider modPan3AmountSlider;
    juce::Label modPan3AmountLabel;


    // =========================
    // MODULATION - LOUDNESS
    // =========================

    juce::Label modAmpSectionLabel;
    juce::Rectangle<int> modAmpCardBounds;

    juce::Label modAmp1Label;
    juce::ComboBox modAmp1SourceCombo;
    juce::Slider modAmp1AmountSlider;
    juce::Label modAmp1AmountLabel;

    juce::Label modAmp2Label;
    juce::ComboBox modAmp2SourceCombo;
    juce::Slider modAmp2AmountSlider;
    juce::Label modAmp2AmountLabel;

    juce::Label modAmp3Label;
    juce::ComboBox modAmp3SourceCombo;


    // =========================
    // MODULATION - LFO1
    // =========================

    juce::Label modLfo1SectionLabel;
    juce::Rectangle<int> modLfo1CardBounds;

    juce::Label modLfo1RateLabel;
    juce::ComboBox modLfo1RateSourceCombo;
    juce::Slider modLfo1RateAmountSlider;
    juce::Label modLfo1RateAmountLabel;

    juce::Label modLfo1DepthLabel;
    juce::ComboBox modLfo1DepthSourceCombo;
    juce::Slider modLfo1DepthAmountSlider;
    juce::Label modLfo1DepthAmountLabel;

    juce::Label modLfo1DelayLabel;
    juce::ComboBox modLfo1DelaySourceCombo;
    juce::Slider modLfo1DelayAmountSlider;
    juce::Label modLfo1DelayAmountLabel;


    // =========================
    // MODULATION - FILTER / PITCH
    // =========================

    juce::Label modFilterPitchSectionLabel;
    juce::Rectangle<int> modFilterPitchCardBounds;

    juce::Label modFilter1Label;
    juce::ComboBox modFilter1SourceCombo;

    juce::Label modFilter2Label;
    juce::ComboBox modFilter2SourceCombo;

    juce::Label modFilter3Label;
    juce::ComboBox modFilter3SourceCombo;

    juce::Label modPitchLabel;
    juce::ComboBox modPitchSourceCombo;
    juce::Slider modPitchAmountSlider;
    juce::Label modPitchAmountLabel;

    juce::Label velocityLoudnessLabel;
    juce::Slider velocityLoudnessSlider;
    juce::Label velocityLoudnessValueLabel;

    juce::Label keygroupCrossfadeLabel;
    juce::ComboBox keygroupCrossfadeCombo;

    juce::Label modWheelDepthLabel;
    juce::Slider modWheelDepthSlider;
    juce::Label modWheelDepthValueLabel;

    juce::Label pressureDepthLabel;
    juce::Slider pressureDepthSlider;
    juce::Label pressureDepthValueLabel;

    juce::Label velocityDepthLabel;
    juce::Slider velocityDepthSlider;
    juce::Label velocityDepthValueLabel;

    //juce::Slider modPitchAmountSlider;
    //juce::Label modPitchAmountLabel;




    juce::Rectangle<int> generalCardBounds;
    juce::Rectangle<int> outputCardBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ProgramEditor
    )
};