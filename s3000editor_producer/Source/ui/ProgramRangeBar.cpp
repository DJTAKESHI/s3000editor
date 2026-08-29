#include "ProgramRangeBar.h"

ProgramRangeBar::ProgramRangeBar()
{
}

void ProgramRangeBar::setRange(
    int low,
    int high)
{
    lowNote =
        juce::jlimit(0, 127, low);

    highNote =
        juce::jlimit(0, 127, high);

    repaint();
}

float ProgramRangeBar::noteToX(
    int note) const
{
    constexpr float margin = 8.0f;

    return juce::jmap(
        static_cast<float>(note),
        0.0f,
        127.0f,
        margin,
        static_cast<float>(getWidth())
        - margin
    );
}

juce::String ProgramRangeBar::noteName(
    int note)
{
    static const char* names[] =
    {
        "C", "C#", "D", "D#",
        "E", "F", "F#", "G",
        "G#", "A", "A#", "B"
    };

    note = juce::jlimit(
        0,
        127,
        note
    );

    const int noteIndex =
        note % 12;

    const int octave =
        note / 12 - 2;

    return
        juce::String(names[noteIndex])
        + juce::String(octave);
}

void ProgramRangeBar::paint(
    juce::Graphics& g)
{
    auto area =
        getLocalBounds()
        .toFloat()
        .reduced(
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
        noteToX(lowNote);

    const float highX =
        noteToX(highNote);

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
        noteName(lowNote),
        0,
        0,
        60,
        20,
        juce::Justification::left
    );

    g.drawText(
        noteName(highNote),
        getWidth() - 60,
        0,
        60,
        20,
        juce::Justification::right
    );
}

int ProgramRangeBar::xToNote(
    float x) const
{
    constexpr float margin = 8.0f;

    const float value =
        juce::jmap(
            x,
            margin,
            static_cast<float>(
                getWidth()
                ) - margin,
            0.0f,
            127.0f
        );

    return juce::jlimit(
        0,
        127,
        juce::roundToInt(value)
    );
}

void ProgramRangeBar::mouseDown(
    const juce::MouseEvent& event)
{
    const float lowX =
        noteToX(lowNote);

    const float highX =
        noteToX(highNote);

    if (std::abs(
        event.position.x - lowX
    )
        <
        std::abs(
            event.position.x - highX
        ))
    {
        draggingLow = true;
    }
    else
    {
        draggingHigh = true;
    }
}

void ProgramRangeBar::mouseDrag(
    const juce::MouseEvent& event)
{
    const int note =
        xToNote(
            event.position.x
        );

    if (draggingLow)
    {
        lowNote =
            juce::jlimit(
                0,
                highNote,
                note
            );
    }

    if (draggingHigh)
    {
        highNote =
            juce::jlimit(
                lowNote,
                127,
                note
            );
    }

    repaint();

    if (onRangeChanged)
    {
        onRangeChanged(
            lowNote,
            highNote
        );
    }
}

void ProgramRangeBar::mouseUp(
    const juce::MouseEvent&)
{
    draggingLow = false;
    draggingHigh = false;

    if (onEditFinished)
    {
        onEditFinished(
            lowNote,
            highNote
        );
    }
}