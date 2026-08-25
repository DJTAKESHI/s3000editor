#include "SamplePositionBar.h"

void SamplePositionBar::setPositions(
    uint64_t length,
    uint32_t start,
    uint32_t end,
    uint32_t loopStart,
    uint32_t loopEnd)
{
    sampleLength = length;

    startPosition = start;
    endPosition = end;

    loopStartPosition = loopStart;
    loopEndPosition = loopEnd;

    repaint();
}

float SamplePositionBar::positionToX(
    uint64_t position) const
{
    constexpr float margin = 10.0f;

    if (sampleLength == 0)
        return margin;

    const auto maxX =
        static_cast<float>(getWidth()) - margin;

    return juce::jmap(
        static_cast<float>(position),
        0.0f,
        static_cast<float>(sampleLength),
        margin,
        maxX
    );
}

void SamplePositionBar::mouseDown(
    const juce::MouseEvent& event)
{
    const float mouseX = event.position.x;

    const float startX =
        positionToX(startPosition);

    const float endX =
        positionToX(endPosition);

    const float loopStartX =
        positionToX(loopStartPosition);

    const float loopEndX =
        positionToX(loopEndPosition);

    const float dStart =
        std::abs(mouseX - startX);

    const float dEnd =
        std::abs(mouseX - endX);

    const float dLoopStart =
        std::abs(mouseX - loopStartX);

    const float dLoopEnd =
        std::abs(mouseX - loopEndX);

    const float nearest =
        juce::jmin(
            juce::jmin(dStart, dEnd),
            juce::jmin(dLoopStart, dLoopEnd)
        );

    if (nearest == dStart)
        draggingStart = true;
    else if (nearest == dEnd)
        draggingEnd = true;
    else if (nearest == dLoopStart)
        draggingLoopStart = true;
    else
        draggingLoopEnd = true;
}

void SamplePositionBar::mouseDrag(
    const juce::MouseEvent& event)
{
    if (sampleLength == 0)
        return;

    const auto newPosition =
        xToPosition(event.position.x);

    if (draggingStart)
    {
        startPosition =
            juce::jlimit<uint32_t>(
                0,
                endPosition,
                newPosition
            );
    }

    if (draggingEnd)
    {
        endPosition =
            juce::jlimit<uint32_t>(
                startPosition,
                static_cast<uint32_t>(
                    sampleLength - 1
                    ),
                newPosition
            );
    }

    if (draggingLoopStart)
    {
        loopStartPosition =
            juce::jlimit<uint32_t>(
                startPosition,
                loopEndPosition,
                newPosition
            );
    }

    if (draggingLoopEnd)
    {
        loopEndPosition =
            juce::jlimit<uint32_t>(
                loopStartPosition,
                endPosition,
                newPosition
            );
    }

    repaint();

    if ((draggingStart || draggingEnd)
        && onRangeChanged)
    {
        DBG(
            "RANGE DRAG START="
            + juce::String(startPosition)
            + " END="
            + juce::String(endPosition)
        );

        onRangeChanged(
            startPosition,
            endPosition
        );
    }

    if ((draggingLoopStart || draggingLoopEnd)
        && onLoopRangeChanged)
    {
        onLoopRangeChanged(
            loopStartPosition,
            loopEndPosition
        );
    }
}

void SamplePositionBar::mouseUp(
    const juce::MouseEvent&)
{
    const bool wasEditing =
        draggingStart ||
        draggingEnd ||
        draggingLoopStart ||
        draggingLoopEnd;

    if (draggingStart || draggingEnd)
    {
        if (onRangeChanged)
        {
            onRangeChanged(
                startPosition,
                endPosition
            );
        }
    }

    if (draggingLoopStart || draggingLoopEnd)
    {
        if (onLoopRangeChanged)
        {
            onLoopRangeChanged(
                loopStartPosition,
                loopEndPosition
            );
        }
    }

    draggingStart = false;
    draggingEnd = false;
    draggingLoopStart = false;
    draggingLoopEnd = false;

    // ドラッグ確定
    if (wasEditing && onEditFinished)
        onEditFinished();
}


uint32_t SamplePositionBar::xToPosition(
    float x) const
{
    constexpr float margin = 10.0f;

    if (sampleLength == 0)
        return 0;

    const float minX = margin;
    const float maxX =
        static_cast<float>(getWidth()) - margin;

    x = juce::jlimit(
        minX,
        maxX,
        x
    );

    const float position =
        juce::jmap(
            x,
            minX,
            maxX,
            0.0f,
            static_cast<float>(sampleLength - 1)
        );

    return static_cast<uint32_t>(
        juce::roundToInt(position)
        );
}


void SamplePositionBar::paint(
    juce::Graphics& g)
{
    auto area =
        getLocalBounds()
        .toFloat()
        .reduced(10.0f, 20.0f);

    // full sample
    g.setColour(
        juce::Colours::darkgrey
    );

    g.fillRoundedRectangle(
        area,
        4.0f
    );

    if (sampleLength == 0)
        return;

    const float startX =
        positionToX(startPosition);

    const float endX =
        positionToX(endPosition);

    const float loopStartX =
        positionToX(loopStartPosition);

    const float loopEndX =
        positionToX(loopEndPosition);

    // active sample range
    g.setColour(
        juce::Colours::cornflowerblue
    );

    g.fillRect(
        startX,
        area.getY(),
        juce::jmax(
            0.0f,
            endX - startX
        ),
        area.getHeight()
    );

    // loop range
    if (loopEndPosition > loopStartPosition)
    {
        g.setColour(
            juce::Colours::orange
        );

        g.fillRect(
            loopStartX,
            area.getY(),
            juce::jmax(
                0.0f,
                loopEndX - loopStartX
            ),
            area.getHeight()
        );

        // Loop Start handle
        g.fillRect(
            loopStartX - 2.0f,
            area.getY() - 6.0f,
            4.0f,
            area.getHeight() + 12.0f
        );

        // Loop End handle
        g.fillRect(
            loopEndX - 2.0f,
            area.getY() - 6.0f,
            4.0f,
            area.getHeight() + 12.0f
        );
    }

    // START marker
    g.setColour(
        juce::Colours::white
    );

    g.fillRect(
        startX - 1.0f,
        area.getY() - 6.0f,
        2.0f,
        area.getHeight() + 12.0f
    );

    // END marker
    g.fillRect(
        endX - 1.0f,
        area.getY() - 6.0f,
        2.0f,
        area.getHeight() + 12.0f
    );

    // labels
    g.drawText(
        "START " + juce::String(startPosition),
        0,
        0,
        getWidth() / 2,
        20,
        juce::Justification::left
    );

    g.drawText(
        "END " + juce::String(endPosition),
        getWidth() / 2,
        0,
        getWidth() / 2,
        20,
        juce::Justification::right
    );
}