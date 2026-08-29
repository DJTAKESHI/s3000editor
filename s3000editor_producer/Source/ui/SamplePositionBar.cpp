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
    auto bounds =
        getLocalBounds().toFloat();

    if (sampleLength == 0)
    {
        g.setColour(
            juce::Colours::grey
            .withAlpha(0.6f)
        );

        g.drawFittedText(
            "No sample",
            getLocalBounds(),
            juce::Justification::centred,
            1
        );

        return;
    }

    const float startX =
        positionToX(startPosition);

    const float endX =
        positionToX(endPosition);

    const float loopStartX =
        positionToX(loopStartPosition);

    const float loopEndX =
        positionToX(loopEndPosition);

    // ========================================
    // VALUE LABELS
    // ========================================

    g.setFont(11.0f);

    g.setColour(
        juce::Colours::lightgrey
    );

    g.drawText(
        "0",
        10,
        0,
        80,
        16,
        juce::Justification::left
    );

    g.drawText(
        "LENGTH "
        + juce::String(
            (juce::int64)sampleLength
        ),
        getWidth() - 150,
        0,
        140,
        16,
        juce::Justification::right
    );

    // ========================================
    // TIMELINE
    // ========================================

    auto timeline =
        bounds.reduced(
            10.0f,
            0.0f
        );

    timeline.setY(31.0f);
    timeline.setHeight(20.0f);

    // Entire sample
    g.setColour(
        juce::Colours::white
        .withAlpha(0.08f)
    );

    g.fillRoundedRectangle(
        timeline,
        4.0f
    );

    // ========================================
    // PLAYBACK RANGE
    // ========================================

    juce::Rectangle<float> playbackRange(
        startX,
        timeline.getY(),
        juce::jmax(
            0.0f,
            endX - startX
        ),
        timeline.getHeight()
    );

    g.setColour(
        juce::Colours::cornflowerblue
        .withAlpha(0.65f)
    );

    g.fillRect(playbackRange);

    // ========================================
    // LOOP RANGE
    // ========================================

    if (loopEndPosition > loopStartPosition)
    {
        // Narrower strip inside playback range
        juce::Rectangle<float> loopRange(
            loopStartX,
            timeline.getY() + 5.0f,
            juce::jmax(
                0.0f,
                loopEndX - loopStartX
            ),
            timeline.getHeight() - 10.0f
        );

        g.setColour(
            juce::Colours::orange
            .withAlpha(0.90f)
        );

        g.fillRoundedRectangle(
            loopRange,
            2.0f
        );
    }

    // ========================================
    // START / END MARKERS
    // ========================================

    g.setColour(
        juce::Colours::white
    );

    g.fillRect(
        startX - 1.0f,
        timeline.getY() - 5.0f,
        2.0f,
        timeline.getHeight() + 10.0f
    );

    g.fillRect(
        endX - 1.0f,
        timeline.getY() - 5.0f,
        2.0f,
        timeline.getHeight() + 10.0f
    );

    // START triangle
    {
        juce::Path marker;

        marker.addTriangle(
            startX - 5.0f,
            timeline.getY() - 8.0f,

            startX + 5.0f,
            timeline.getY() - 8.0f,

            startX,
            timeline.getY() - 2.0f
        );

        g.fillPath(marker);
    }

    // END triangle
    {
        juce::Path marker;

        marker.addTriangle(
            endX - 5.0f,
            timeline.getY() - 8.0f,

            endX + 5.0f,
            timeline.getY() - 8.0f,

            endX,
            timeline.getY() - 2.0f
        );

        g.fillPath(marker);
    }

    // ========================================
    // LOOP MARKERS
    // ========================================

    if (loopEndPosition > loopStartPosition)
    {
        g.setColour(
            juce::Colours::orange
        );

        g.fillRect(
            loopStartX - 1.0f,
            timeline.getY() + 2.0f,
            2.0f,
            timeline.getHeight() - 4.0f
        );

        g.fillRect(
            loopEndX - 1.0f,
            timeline.getY() + 2.0f,
            2.0f,
            timeline.getHeight() - 4.0f
        );
    }

    // ========================================
    // BOTTOM LABELS
    // ========================================

    g.setFont(10.0f);

    g.setColour(
        juce::Colours::white
        .withAlpha(0.9f)
    );

    g.drawText(
        "START "
        + juce::String(startPosition),
        10,
        57,
        getWidth() / 2 - 10,
        16,
        juce::Justification::left
    );

    g.drawText(
        "END "
        + juce::String(endPosition),
        getWidth() / 2,
        57,
        getWidth() / 2 - 10,
        16,
        juce::Justification::right
    );

    // Loop information in the centre
    if (loopEndPosition > loopStartPosition)
    {
        g.setColour(
            juce::Colours::orange
        );

        g.drawText(
            "LOOP "
            + juce::String(loopStartPosition)
            + " - "
            + juce::String(loopEndPosition),
            80,
            57,
            getWidth() - 160,
            16,
            juce::Justification::centred
        );
    }
}