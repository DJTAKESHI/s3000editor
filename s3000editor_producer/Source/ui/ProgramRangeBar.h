#pragma once

#include <JuceHeader.h>

class ProgramRangeBar
    : public juce::Component
{
public:
    ProgramRangeBar();

    void setRange(
        int low,
        int high
    );

    void paint(
        juce::Graphics& g
    ) override;

    std::function<void(int low, int high)>
        onRangeChanged;

    void mouseDown(
        const juce::MouseEvent& event
    ) override;

    void mouseDrag(
        const juce::MouseEvent& event
    ) override;

    void mouseUp(
        const juce::MouseEvent& event
    ) override;

    std::function<void(int low, int high)>
        onEditFinished;



private:
    int lowNote = 0;
    int highNote = 127;

    float noteToX(
        int note
    ) const;

    static juce::String noteName(
        int note
    );

    bool draggingLow = false;
    bool draggingHigh = false;

    int xToNote(
        float x
    ) const;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ProgramRangeBar
    )
};