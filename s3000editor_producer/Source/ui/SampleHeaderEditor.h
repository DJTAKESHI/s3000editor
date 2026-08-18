#pragma once

#include <JuceHeader.h>
#include "../s3000/S3000Types.h"
#include "../s3000/Offsets.h"

class SampleHeaderEditor : public juce::Component
{
public:
    SampleHeaderEditor();

    void setSampleHeader(const SampleHeader& header);

    void resized() override;

    SampleHeader currentHeader;
    std::function<void(const SampleHeader&)> onSampleHeaderChanged;
    void updateLoopDwellStatus();
    juce::String midiNoteName(int note);

private:

    juce::Label titleLabel;

    juce::Label idLabel;
    juce::Label nameLabel;
    juce::Label bandwidthLabel;
    juce::Label pitchLabel;
    juce::Label sampleRateValidLabel;
    juce::Label numLoopsLabel;
    juce::Label activeLoopLabel;
    juce::Label highestLoopLabel;
    juce::Label playTypeLabel;
    juce::Label tuneLabel;
    juce::Label locationLabel;
    juce::Label lengthLabel;
    juce::Label startLabel;
    juce::Label endLabel;
    juce::Label loopsLabel;
    juce::Label pitchNoteLabel;

    //juce::Label loop1PositionLabel;
    ////juce::TextEditor loop1PositionEditor;

    //juce::Label loop1LengthLabel;
    //juce::TextEditor loop1LengthEditor;

    //juce::Label loop1DwellLabel;
    //juce::TextEditor loop1DwellEditor;


    juce::Label sampleRateLabel;
    juce::TextEditor sampleRateEditor;

    juce::Label holdLoopTuneLabel;
    juce::TextEditor holdLoopTuneEditor;

    juce::TextEditor idEditor;
    juce::TextEditor nameEditor;
    //juce::TextEditor bandwidthEditor;
    juce::ComboBox bandwidthCombo;
    juce::TextEditor pitchEditor;
    //juce::TextEditor sampleRateValidEditor;
    juce::ComboBox sampleRateValidCombo;

    juce::ComboBox numLoopsCombo;
    //juce::TextEditor activeLoopEditor;
    juce::ComboBox activeLoopCombo;
    juce::TextEditor highestLoopEditor;
    juce::ComboBox playTypeCombo;
    juce::TextEditor tuneEditor;
    juce::TextEditor locationEditor;
    juce::TextEditor lengthEditor;
    juce::TextEditor startEditor;
    juce::TextEditor endEditor;
    juce::TextEditor loopsEditor;

    //juce::TextEditor playTypeEditor;

    //SampleHeader currentHeader;

    juce::Label loopSelectLabel;
    juce::ComboBox loopSelectCombo;

    juce::Label loopPositionLabel;
    juce::TextEditor loopPositionEditor;

    juce::Label loopLengthLabel;
    juce::TextEditor loopLengthEditor;

    juce::Label loopDwellLabel;
    juce::TextEditor loopDwellEditor;

    juce::Label loopDwellStatusLabel;


    int selectedLoopIndex = 0;
    bool hasValidHeader = false;

    //juce::Label playTypeLabel;
    //juce::ComboBox playTypeCombo;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        SampleHeaderEditor
    )
};
