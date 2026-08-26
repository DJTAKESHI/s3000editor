#include "VelocityZoneMap.h"

void VelocityZoneMap::setZones(
    const std::array<VelocityZone, 4>& newZones)
{
    DBG("===== VelocityZoneMap::setZones CALLED =====");

    zones = newZones;

    repaint();
}

void VelocityZoneMap::paint(
    juce::Graphics& g)
{
    auto area =
        getLocalBounds()
        .toFloat()
        .reduced(8.0f);

    constexpr float rowHeight = 30.0f;
    constexpr float labelWidth = 55.0f;
    constexpr float textWidth = 120.0f;

    for (int i = 0; i < 4; ++i)
    {
        const float y =
            area.getY()
            + i * rowHeight;

        if (i == selectedZone)
        {
            g.setColour(
                juce::Colours::white
                .withAlpha(0.12f)
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

        // =========================
        // Zone label
        // =========================

        g.setColour(
            juce::Colours::white
        );

        g.drawText(
            "Zone " + juce::String(i + 1),
            static_cast<int>(area.getX()),
            static_cast<int>(y),
            static_cast<int>(labelWidth),
            static_cast<int>(rowHeight),
            juce::Justification::centredLeft
        );

        auto barArea =
            juce::Rectangle<float>(
                area.getX() + labelWidth,
                y + 5.0f,
                area.getWidth()
                - labelWidth
                - textWidth,
                rowHeight - 10.0f
            );

        DBG(
            "ZONE MAP GEOMETRY "
            + juce::String(i)
            + " COMPONENT WIDTH="
            + juce::String(getWidth())
            + " BAR X="
            + juce::String(barArea.getX())
            + " BAR WIDTH="
            + juce::String(barArea.getWidth())
        );

        // Background
        g.setColour(
            juce::Colours::darkgrey
        );

        g.fillRoundedRectangle(
            barArea,
            3.0f
        );

        if (i >= (int)zones.size())
            continue;

        const auto& zone =
            zones[i];

        DBG(
            "ZONE MAP "
            + juce::String(i)
            + " SAMPLE=["
            + zone.sampleName
            + "] LOW="
            + juce::String(zone.lowVel)
            + " HIGH="
            + juce::String(zone.highVel)
        );

        const bool hasSample =
            !zone.sampleName.trim().isEmpty();

        if (!hasSample)
        {
            g.setColour(
                juce::Colours::grey
            );

            g.drawText(
                "---",
                static_cast<int>(
                    barArea.getRight() + 8.0f
                    ),
                static_cast<int>(y),
                static_cast<int>(textWidth - 8.0f),
                static_cast<int>(rowHeight),
                juce::Justification::centredLeft
            );

            continue;
        }

        // =========================
        // Velocity range
        // =========================

        const float x1 =
            barArea.getX()
            + static_cast<float>(zone.lowVel)
            / 128.0f
            * barArea.getWidth();

        const float x2 =
            barArea.getX()
            + static_cast<float>(zone.highVel + 1)
            / 128.0f
            * barArea.getWidth();

        juce::Rectangle<float> active(
            x1,
            barArea.getY(),
            juce::jmax(
                4.0f,
                x2 - x1
            ),
            barArea.getHeight()
        );

        DBG(
            "ZONE MAP ACTIVE "
            + juce::String(i)
            + " X="
            + juce::String(active.getX())
            + " Y="
            + juce::String(active.getY())
            + " W="
            + juce::String(active.getWidth())
            + " H="
            + juce::String(active.getHeight())
        );


        g.setColour(
            juce::Colours::cornflowerblue
        );

        g.fillRoundedRectangle(
            active,
            3.0f
        );

        // TEST: activeóÃàÊÇÃòg
        g.setColour(
            juce::Colours::white
        );

        // =========================
        // Range + Sample
        // =========================

        const juce::String text =
            juce::String(zone.lowVel)
            + "-"
            + juce::String(zone.highVel)
            + "  "
            + zone.sampleName.trim();

        g.setColour(
            juce::Colours::white
        );

        g.drawText(
            text,
            static_cast<int>(
                barArea.getRight() + 8.0f
                ),
            static_cast<int>(y),
            static_cast<int>(
                textWidth - 8.0f
                ),
            static_cast<int>(rowHeight),
            juce::Justification::centredLeft
        );
    }
}

void VelocityZoneMap::setSelectedZone(int index)
{
    selectedZone =
        juce::jlimit(0, 3, index);

    repaint();
}

void VelocityZoneMap::mouseDown(
    const juce::MouseEvent& event)
{
    constexpr float rowHeight = 30.0f;

    // paint() ÇÃ reduced(8.0f) Ç∆çáÇÌÇπÇÈ
    const float top = 8.0f;

    const int zoneIndex =
        static_cast<int>(
            (event.position.y - top)
            / rowHeight
            );

    if (zoneIndex < 0 ||
        zoneIndex >= 4)
    {
        return;
    }

    DBG(
        "VELOCITY ZONE MAP SELECTED ZONE="
        + juce::String(zoneIndex)
    );

    if (onZoneSelected)
    {
        onZoneSelected(
            zoneIndex
        );
    }
}