#pragma once

#include <JuceHeader.h>
#include "../s3000/s3000Types.h"  

class VelocityZoneEditor : public juce::Component
{
public:
    VelocityZoneEditor();

    void setZone(const VelocityZone& zone);

    std::function<void(const VelocityZone&)> onZoneChanged;

    void setResidentSamples(
        const std::map<int, juce::String>& samples
    );


    void resized() override;

private:
    VelocityZone currentZone;

    juce::Label titleLabel;

    juce::Label sampleNameLabel;
    //juce::TextEditor sampleNameEditor;
    juce::ComboBox sampleCombo;
    //const std::map<int, juce::String>* residentSamples = nullptr;
    //std::vector<juce::String> residentSamples;
    std::map<int, juce::String> residentSamples;

    juce::Label sampleIdLabel;
    juce::TextEditor sampleIdEditor;

    juce::Label lowVelLabel;
    juce::TextEditor lowVelEditor;

    juce::Label highVelLabel;
    juce::TextEditor highVelEditor;

    juce::Label semitoneLabel;
    juce::TextEditor semitoneEditor;

    juce::Label fineTuneLabel;
    juce::TextEditor fineTuneEditor;

    juce::Label loudnessLabel;
    juce::TextEditor loudnessEditor;

    juce::Label panLabel;
    juce::TextEditor panEditor;

    juce::Label playModeLabel;
    juce::ComboBox playModeCombo;

    juce::Label filterFreqLabel;
    juce::TextEditor filterFreqEditor;

    //juce::Label lowVelXFadeLabel;
    //juce::TextEditor lowVelXFadeEditor;

    //juce::Label highVelXFadeLabel;
    //juce::TextEditor highVelXFadeEditor;

};