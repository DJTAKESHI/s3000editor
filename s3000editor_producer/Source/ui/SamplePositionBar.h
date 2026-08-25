#pragma once

#include <JuceHeader.h>

class SamplePositionBar : public juce::Component
{
public:
    SamplePositionBar() = default;

    void setPositions(
        uint64_t length,
        uint32_t start,
        uint32_t end,
        uint32_t loopStart,
        uint32_t loopEnd);

    std::function<void(uint32_t start, uint32_t end)>
        onRangeChanged;

    void paint(juce::Graphics& g) override;

    void mouseDown(
        const juce::MouseEvent& event) override;

    void mouseDrag(
        const juce::MouseEvent& event) override;

    void mouseUp(
        const juce::MouseEvent& event) override;

    std::function<void(uint32_t loopStart, uint32_t loopEnd)>
        onLoopRangeChanged;

    std::function<void()> onEditFinished;

private:
    uint64_t sampleLength = 0;

    uint32_t startPosition = 0;
    uint32_t endPosition = 0;

    uint32_t loopStartPosition = 0;
    uint32_t loopEndPosition = 0;

    bool draggingStart = false;
    bool draggingEnd = false;

    float positionToX(uint64_t position) const;
    uint32_t xToPosition(float x) const;

    bool draggingLoopStart = false;
    bool draggingLoopEnd = false;

    

};