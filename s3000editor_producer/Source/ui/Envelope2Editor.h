#pragma once
#include <JuceHeader.h>

class Envelope2Editor : public juce::Component
{
public:
    Envelope2Editor();

    void paint(juce::Graphics& g) override;

    void setEnvelope(
        int r1, int l1,
        int r2, int l2,
        int r3, int l3,
        int r4, int l4
    );

    void mouseDown(
        const juce::MouseEvent& event) override;

    void mouseDrag(
        const juce::MouseEvent& event) override;

    void mouseUp(
        const juce::MouseEvent& event) override;

    std::function<void(
        int r1, int l1,
        int r2, int l2,
        int r3, int l3,
        int r4, int l4)> onEnvelopeChanged;

    std::function<void()> onEditFinished;

private:
    int r1Value = 0;
    int l1Value = 0;

    int r2Value = 0;
    int l2Value = 0;

    int r3Value = 0;
    int l3Value = 0;

    int r4Value = 0;
    int l4Value = 0;

    enum class DragPoint
    {
        none,
        point1,
        point2,
        point3,
        point4
    };

    DragPoint dragPoint = DragPoint::none;

    juce::Rectangle<float> getGraphBounds() const;

    juce::Point<float> getPoint1() const;
    juce::Point<float> getPoint2() const;
    juce::Point<float> getPoint3() const;
    juce::Point<float> getPoint4() const;

    void notifyEnvelopeChanged();

    
    

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        Envelope2Editor
    )
};