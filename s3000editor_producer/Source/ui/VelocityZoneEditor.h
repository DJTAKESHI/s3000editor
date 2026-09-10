#pragma once

#include <JuceHeader.h>
#include "VelocityRangeBar.h"
#include "../s3000/s3000Types.h"  
#include "VelocityZoneMap.h"

class VelocityZoneEditor : public juce::Component
{
public:
    VelocityZoneEditor();
    VelocityZoneMap velocityZoneMap;

    void setZone(const VelocityZone& zone);
    void setZones(
        const std::array<VelocityZone, 4>& zones
    );

    std::function<void(const VelocityZone&)> onZoneChanged;

    void setResidentSamples(
        const std::map<int, juce::String>& samples
    );

    std::function<void(int)>
        onZoneSelected;

    std::function<void(
        int zoneIndex,
        int low,
        int high
        )> onZoneRangeChanged;

    std::function<void(bool)> onTrackingChanged;

    std::function<void(int)> onSemitoneChanged;
    std::function<void(int)> onFineTuneChanged;

    std::function<void(int)> onLoudnessChanged;
    std::function<void(int)> onPanChanged;
    std::function<void(int)> onFilterFreqChanged;
    std::function<void(int, int)> onVelocityRangeChanged;

    void setLoudness(int value);
    void setPan(int value);
    void setFilterFreq(int value);

    void setLowVelocity(int value);
    void setHighVelocity(int value);



    void resized() override;

    void setSelectedZone(int index);

    void paint(juce::Graphics& g) override;

private:
    
    VelocityZone currentZone;
    VelocityRangeBar velocityRangeBar;

    static constexpr int noneSampleComboId = 10000;

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
    //juce::TextEditor semitoneEditor;
    juce::Slider semitoneSlider;

    juce::Label fineTuneLabel;
    //juce::TextEditor fineTuneEditor;
    juce::Slider fineTuneSlider;

    juce::Label loudnessLabel;
    juce::Slider loudnessSlider;

    juce::Label panLabel;
    juce::Slider panSlider;

    juce::Label playModeLabel;
    juce::ComboBox playModeCombo;

    juce::Label pitchTrackingLabel;
    juce::ComboBox pitchTrackingCombo;

    juce::Label filterFreqLabel;
    juce::Slider filterFreqSlider;

    juce::Label sampleSectionLabel;
    juce::Label velocitySectionLabel;
    juce::Label pitchSectionLabel;
    juce::Label outputSectionLabel;

    juce::Rectangle<int> sampleCardBounds;
    juce::Rectangle<int> velocityCardBounds;
    juce::Rectangle<int> pitchCardBounds;
    juce::Rectangle<int> outputCardBounds;



    //juce::Label lowVelXFadeLabel;
    //juce::TextEditor lowVelXFadeEditor;

    //juce::Label highVelXFadeLabel;
    //juce::TextEditor highVelXFadeEditor;

};