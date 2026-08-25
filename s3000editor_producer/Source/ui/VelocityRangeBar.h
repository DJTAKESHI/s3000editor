#pragma once

#include <JuceHeader.h>

class VelocityRangeBar : public juce::Component
{
public:
    VelocityRangeBar();

    void setRange(int low, int high);

    std::function<void(int low, int high)>
        onRangeChanged;

    void paint(juce::Graphics& g) override;

    void mouseDown(
        const juce::MouseEvent& event) override;

    void mouseDrag(
        const juce::MouseEvent& event) override;

    void mouseUp(
        const juce::MouseEvent& event) override;

private:
    int lowVelocity = 0;
    int highVelocity = 127;

    bool draggingLow = false;
    bool draggingHigh = false;

    float velocityToX(int velocity) const;
    int xToVelocity(float x) const;
};