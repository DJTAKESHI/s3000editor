#include "KeygroupMap.h"

void KeygroupMap::setProgram(
    const Program& program)
{
    currentProgram = program;



    for (int i = 0;
        i < (int)currentProgram.keygroups.size();
        ++i)
    {
        const auto& kg =
            currentProgram.keygroups[i];

    }

    if (!currentProgram.keygroups.empty() &&
        selectedKeygroup < 0)
    {
        selectedKeygroup = 0;
    }

    const int contentHeight =
        juce::jmax(
            100,
            static_cast<int>(
                currentProgram.keygroups.size()
                ) * rowHeight + 20
        );

    setSize(
        getWidth(),
        contentHeight
    );

    repaint();
}

void KeygroupMap::resized()
{
    repaint();
}


float KeygroupMap::noteToX(
    int note) const
{
    auto mapArea = getMapArea();

    constexpr float labelWidth = 55.0f;
    constexpr float rangeWidth = 80.0f;

    mapArea.removeFromLeft(labelWidth);
    mapArea.removeFromRight(rangeWidth);

    const float normalized =
        juce::jlimit(
            0.0f,
            1.0f,
            static_cast<float>(note) / 128.0f
        );

    return mapArea.getX()
        + normalized * mapArea.getWidth();
}

void KeygroupMap::paint(
    juce::Graphics& g)
{
    auto area = getMapArea();

    g.setColour(
        juce::Colours::darkgrey
    );

    g.fillRoundedRectangle(
        area,
        4.0f
    );

    if (currentProgram.keygroups.empty())
        return;

    const int count =
        static_cast<int>(
            currentProgram.keygroups.size()
            );

    //const float rowHeight = 32.0f;
    const float labelWidth = 55.0f;



    for (int i = 0; i < count; ++i)
    {
        const auto& kg =
            currentProgram.keygroups[i];

        const float y =
            area.getY()
            + i * rowHeight; 

        if (i == selectedKeygroup)
        {
            g.setColour(
                juce::Colours::white
                .withAlpha(0.10f)
            );

            g.fillRoundedRectangle(
                juce::Rectangle<float>(
                    area.getX(),
                    y,
                    area.getWidth(),
                    rowHeight
                ),
                4.0f
            );
        }

        const auto bar =
            getBarBounds(i);





        // =========================
        // KG label
        // =========================

        g.setColour(
            juce::Colours::white
        );

        g.drawText(
            "KG " + juce::String(i + 1),
            static_cast<int>(area.getX()),
            static_cast<int>(y),
            static_cast<int>(labelWidth),
            static_cast<int>(rowHeight),
            juce::Justification::centredLeft
        );

        // =========================
        // Key range bar
        // =========================



        if (i == selectedKeygroup)
        {
            g.setColour(
                juce::Colours::cornflowerblue
            );
        }
        else
        {
            g.setColour(
                juce::Colours::grey
            );
        }

        g.fillRoundedRectangle(
            bar,
            3.0f
        );

        // ========================================
// Selected Keygroup handles
// ========================================

        if (i == selectedKeygroup)
        {
            constexpr float handleVisualWidth = 6.0f;

            g.setColour(
                juce::Colours::white
                .withAlpha(0.9f)
            );

            // LOW handle
            juce::Rectangle<float> lowHandle(
                bar.getX()
                - handleVisualWidth / 2.0f,
                bar.getY(),
                handleVisualWidth,
                bar.getHeight()
            );

            // HIGH handle
            juce::Rectangle<float> highHandle(
                bar.getRight()
                - handleVisualWidth / 2.0f,
                bar.getY(),
                handleVisualWidth,
                bar.getHeight()
            );

            g.fillRoundedRectangle(
                lowHandle,
                2.0f
            );

            g.fillRoundedRectangle(
                highHandle,
                2.0f
            );
        }


        const auto lowName =
            juce::MidiMessage::getMidiNoteName(
                kg.lowNote,
                true,
                true,
                3
            );

        const auto highName =
            juce::MidiMessage::getMidiNoteName(
                kg.highNote,
                true,
                true,
                3
            );

        const juce::String rangeText =
            lowName + "-" + highName;

        g.setColour(
            juce::Colours::white
        );

        g.drawText(
            rangeText,
            static_cast<int>(bar.getRight() + 8.0f),
            static_cast<int>(y),
            80,
            static_cast<int>(rowHeight),
            juce::Justification::centredLeft
        );


    }
}

int KeygroupMap::keygroupAtPosition(
    juce::Point<float> position) const
{
    auto area = getMapArea();

    if (!area.contains(position))
        return -1;

    const int count =
        static_cast<int>(
            currentProgram.keygroups.size()
            );

    if (count == 0)
        return -1;

    const int index =
        static_cast<int>(
            (position.y - area.getY())
            / static_cast<float>(rowHeight)
            );

    if (index < 0 || index >= count)
        return -1;

    return index;
}

void KeygroupMap::mouseDown(
    const juce::MouseEvent& event)
{
    const int index =
        keygroupAtPosition(
            event.position
        );

    if (index < 0)
        return;

    selectedKeygroup = index;

    const auto bar =
        getBarBounds(index);

    const float x =
        event.position.x;

    constexpr float hitWidth = 20.0f;

    juce::Rectangle<float> lowHitArea(
        bar.getX() - hitWidth / 2.0f,
        bar.getY(),
        hitWidth,
        bar.getHeight()
    );

    juce::Rectangle<float> highHitArea(
        bar.getRight() - hitWidth / 2.0f,
        bar.getY(),
        hitWidth,
        bar.getHeight()
    );

    const bool hitLow =
        lowHitArea.contains(event.position);

    const bool hitHigh =
        highHitArea.contains(event.position);

    if (hitLow && hitHigh)
    {
        // 短いバーで両方の領域が重なった場合だけ
        // 本当に近い方を選択
        const float lowDistance =
            std::abs(
                event.position.x - bar.getX()
            );

        const float highDistance =
            std::abs(
                event.position.x - bar.getRight()
            );

        dragMode =
            lowDistance <= highDistance
            ? DragMode::lowNote
            : DragMode::highNote;
    }
    else if (hitLow)
    {
        dragMode =
            DragMode::lowNote;
    }
    else if (hitHigh)
    {
        dragMode =
            DragMode::highNote;
    }
    else
    {
        dragMode =
            DragMode::none;
    }

    repaint();

    if (onKeygroupSelected)
        onKeygroupSelected(index);
}

KeygroupMap::KeygroupMap()
{

}


void KeygroupMap::mouseDrag(
    const juce::MouseEvent& event)
{
    if (selectedKeygroup < 0 ||
        selectedKeygroup >=
        static_cast<int>(
            currentProgram.keygroups.size()
            ))
    {
        return;
    }

    if (dragMode == DragMode::none)
        return;

    auto& kg =
        currentProgram.keygroups[
            selectedKeygroup
        ];

    const int note =
        noteFromX(event.position.x);

    if (dragMode == DragMode::lowNote)
    {
        kg.lowNote =
            juce::jmin(
                note,
                static_cast<int>(kg.highNote)
            );
    }
    else if (
        dragMode == DragMode::highNote)
    {
        kg.highNote =
            juce::jmax(
                note,
                static_cast<int>(kg.lowNote)
            );
    }

    repaint();

    if (onRangeChanged)
    {
        onRangeChanged(
            selectedKeygroup,
            kg.lowNote,
            kg.highNote
        );
    }
}

void KeygroupMap::mouseUp(
    const juce::MouseEvent&)
{
    if (dragMode == DragMode::none)
        return;

    if (selectedKeygroup >= 0 &&
        selectedKeygroup <
        static_cast<int>(
            currentProgram.keygroups.size()
            ))
    {
        const auto& kg =
            currentProgram.keygroups[
                selectedKeygroup
            ];

        DBG(
            "KEYGROUP RANGE EDIT FINISHED KG="
            + juce::String(selectedKeygroup)
            + " LOW="
            + juce::String(kg.lowNote)
            + " HIGH="
            + juce::String(kg.highNote)
        );

        if (onRangeChangeFinished)
        {
            onRangeChangeFinished(
                selectedKeygroup,
                kg.lowNote,
                kg.highNote
            );
        }
    }

    dragMode =
        DragMode::none;
}



juce::Rectangle<float>
KeygroupMap::getBarBounds(
    int keygroupIndex) const
{
    if (keygroupIndex < 0 ||
        keygroupIndex >=
        static_cast<int>(
            currentProgram.keygroups.size()
            ))
    {
        return {};
    }

    auto area = getMapArea();

    constexpr float labelWidth = 55.0f;
    constexpr float rangeWidth = 80.0f;

    auto mapArea = area;

    mapArea.removeFromLeft(labelWidth);
    mapArea.removeFromRight(rangeWidth);

    const auto& kg =
        currentProgram.keygroups[keygroupIndex];

    const float x1 =
        mapArea.getX()
        + (static_cast<float>(kg.lowNote) / 128.0f)
        * mapArea.getWidth();

    const float x2 =
        mapArea.getX()
        + (static_cast<float>(kg.highNote + 1) / 128.0f)
        * mapArea.getWidth();

    const float y =
        area.getY()
        + keygroupIndex * rowHeight;

    return {
        x1,
        y + 3.0f,
        juce::jmax(4.0f, x2 - x1),
        static_cast<float>(rowHeight) - 6.0f
    };
}

juce::Rectangle<float> KeygroupMap::getMapArea() const
{
    return getLocalBounds()
        .toFloat()
        .reduced(10.0f);
}


int KeygroupMap::noteFromX(
    float x) const
{
    auto mapArea = getMapArea();

    constexpr float labelWidth = 55.0f;
    constexpr float rangeWidth = 80.0f;

    mapArea.removeFromLeft(labelWidth);
    mapArea.removeFromRight(rangeWidth);

    const float normalized =
        juce::jlimit(
            0.0f,
            1.0f,
            (x - mapArea.getX())
            / mapArea.getWidth()
        );

    return juce::jlimit(
        0,
        127,
        static_cast<int>(
            normalized * 128.0f
            )
    );
}