#include "VelocityRangeBar.h"

VelocityRangeBar::VelocityRangeBar()
{
}

void VelocityRangeBar::setRange(
    int low,
    int high)
{
    lowVelocity =
        juce::jlimit(0, 127, low);

    highVelocity =
        juce::jlimit(0, 127, high);

    repaint();
}

float VelocityRangeBar::velocityToX(
    int velocity) const
{
    constexpr float margin = 8.0f;

    return juce::jmap(
        static_cast<float>(velocity),
        0.0f,
        127.0f,
        margin,
        static_cast<float>(getWidth())
        - margin
    );
}

int VelocityRangeBar::xToVelocity(
    float x) const
{
    constexpr float margin = 8.0f;

    const float value =
        juce::jmap(
            x,
            margin,
            static_cast<float>(getWidth())
            - margin,
            0.0f,
            127.0f
        );

    return juce::jlimit(
        0,
        127,
        juce::roundToInt(value)
    );
}

void VelocityRangeBar::paint(
    juce::Graphics& g)
{
    auto area =
        getLocalBounds().toFloat().reduced(
            8.0f,
            20.0f
        );

    g.setColour(
        juce::Colours::darkgrey
    );

    g.fillRoundedRectangle(
        area,
        4.0f
    );

    const float lowX =
        velocityToX(lowVelocity);

    const float highX =
        velocityToX(highVelocity);

    juce::Rectangle<float> activeArea(
        lowX,
        area.getY(),
        highX - lowX,
        area.getHeight()
    );

    g.setColour(
        juce::Colours::cornflowerblue
    );

    g.fillRoundedRectangle(
        activeArea,
        4.0f
    );

    g.setColour(
        juce::Colours::white
    );

    g.fillRect(
        lowX - 2.0f,
        area.getY() - 4.0f,
        4.0f,
        area.getHeight() + 8.0f
    );

    g.fillRect(
        highX - 2.0f,
        area.getY() - 4.0f,
        4.0f,
        area.getHeight() + 8.0f
    );

    g.drawText(
        juce::String(lowVelocity),
        0,
        0,
        50,
        20,
        juce::Justification::left
    );

    g.drawText(
        juce::String(highVelocity),
        getWidth() - 50,
        0,
        50,
        20,
        juce::Justification::right
    );
}

void VelocityRangeBar::mouseDown(
    const juce::MouseEvent& event)
{
    const float lowX =
        velocityToX(lowVelocity);

    const float highX =
        velocityToX(highVelocity);

    if (std::abs(event.position.x - lowX)
        <
        std::abs(event.position.x - highX))
    {
        draggingLow = true;
    }
    else
    {
        draggingHigh = true;
    }
}

void VelocityRangeBar::mouseDrag(
    const juce::MouseEvent& event)
{
    const int velocity =
        xToVelocity(event.position.x);

    if (draggingLow)
    {
        lowVelocity =
            juce::jlimit(
                0,
                highVelocity,
                velocity
            );
    }

    if (draggingHigh)
    {
        highVelocity =
            juce::jlimit(
                lowVelocity,
                127,
                velocity
            );
    }

    repaint();

    //if (onRangeChanged)
    //{
    //    onRangeChanged(
    //        lowVelocity,
    //        highVelocity
    //    );
    //}
}

void VelocityRangeBar::mouseUp(
    const juce::MouseEvent&)
{
    draggingLow = false;
    draggingHigh = false;

    if (onRangeChanged)
    {
        onRangeChanged(
            lowVelocity,
            highVelocity
        );
    }
}

