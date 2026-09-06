#pragma once

#include <JuceHeader.h>

class EnvelopeEditor : public juce::Component
{
public:
    EnvelopeEditor();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseDown(
        const juce::MouseEvent& event) override;

    void mouseDrag(
        const juce::MouseEvent& event) override;

    void mouseUp(
        const juce::MouseEvent& event) override;

    void setEnvelope(
        int attack,
        int decay,
        int sustain,
        int release);

    std::function<void(
        int attack,
        int decay,
        int sustain,
        int release)> onEnvelopeChanged;

    std::function<void()> onEditFinished;

    

private:
    int attackValue = 0;
    int decayValue = 0;
    int sustainValue = 100;
    int releaseValue = 0;

    enum class DragPoint
    {
        none,
        attack,
        decay,
        sustain,
        release
    };

    DragPoint dragPoint = DragPoint::none;

    juce::Rectangle<float> getGraphBounds() const;

    juce::Point<float> getAttackPoint() const;
    juce::Point<float> getDecayPoint() const;
    juce::Point<float> getSustainPoint() const;
    juce::Point<float> getReleasePoint() const;

    void notifyEnvelopeChanged();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        EnvelopeEditor
    )
};