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

    constexpr float rowHeight = 22.0f;
    constexpr float labelWidth = 55.0f;
    constexpr float rangeTextWidth = 65.0f;
    constexpr float sampleTextWidth = 130.0f;

    const float textWidth =
        rangeTextWidth + sampleTextWidth;

    for (int i = 0; i < 4; ++i)
    {
        const float y =
            area.getY()
            + i * rowHeight;

        if (i == selectedZone)
        {
            g.setColour(
                juce::Colour::fromRGB(
                    190, 45, 38
                ).withAlpha(0.10f)
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
            juce::Colour::fromRGB(
                55, 55, 52
            )
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



        g.setColour(
            juce::Colour::fromRGB(
                190, 45, 38
            )
        );

        g.fillRoundedRectangle(
            active,
            3.0f
        );

        // TEST: active—Ìˆæ‚Ì˜g
        g.setColour(
            juce::Colours::white
        );

        // =========================
// Range + Sample
// =========================

        const float textX =
            barArea.getRight() + 8.0f;

        g.setColour(
            juce::Colour::fromRGB(
                55, 55, 52
            )
        );

        // Velocity Range
        g.drawText(
            juce::String(zone.lowVel)
            + "-"
            + juce::String(zone.highVel),
            static_cast<int>(textX),
            static_cast<int>(y),
            static_cast<int>(rangeTextWidth),
            static_cast<int>(rowHeight),
            juce::Justification::centredLeft
        );

        // Sample Name
        g.drawText(
            zone.sampleName.trim(),
            static_cast<int>(
                textX + rangeTextWidth
                ),
            static_cast<int>(y),
            static_cast<int>(sampleTextWidth),
            static_cast<int>(rowHeight),
            juce::Justification::centredLeft
        );
    }
}

int VelocityZoneMap::xToVelocity(
    float x,
    const juce::Rectangle<float>& barArea
) const
{
    const float normalized =
        juce::jlimit(
            0.0f,
            1.0f,
            (x - barArea.getX())
            / barArea.getWidth()
        );

    return juce::jlimit(
        0,
        127,
        juce::roundToInt(
            normalized * 127.0f
        )
    );
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
    constexpr float rowHeight = 22.0f;
    constexpr float labelWidth = 55.0f;
    constexpr float rangeTextWidth = 65.0f;
    constexpr float sampleTextWidth = 160.0f;

    const float textWidth =
        rangeTextWidth
        + sampleTextWidth;

    auto area =
        getLocalBounds()
        .toFloat()
        .reduced(8.0f);

    const int zoneIndex =
        static_cast<int>(
            (event.position.y - area.getY())
            / rowHeight
            );

    if (zoneIndex < 0 ||
        zoneIndex >= 4)
    {
        return;
    }

    if (onZoneSelected)
        onZoneSelected(zoneIndex);

    const auto& zone =
        zones[zoneIndex];

    auto barArea =
        juce::Rectangle<float>(
            area.getX() + labelWidth,
            area.getY()
            + zoneIndex * rowHeight
            + 5.0f,
            area.getWidth()
            - labelWidth
            - textWidth,
            rowHeight - 10.0f
        );

    const float lowX =
        barArea.getX()
        + static_cast<float>(zone.lowVel)
        / 128.0f
        * barArea.getWidth();

    const float highX =
        barArea.getX()
        + static_cast<float>(
            zone.highVel + 1
            )
        / 128.0f
        * barArea.getWidth();

    draggingZone =
        zoneIndex;

    if (std::abs(
        event.position.x - lowX
    )
        <
        std::abs(
            event.position.x - highX
        ))
    {
        draggingLow = true;
        draggingHigh = false;
    }
    else
    {
        draggingLow = false;
        draggingHigh = true;
    }
}

void VelocityZoneMap::mouseDrag(
    const juce::MouseEvent& event)
{
    if (draggingZone < 0 ||
        draggingZone >= 4)
    {
        return;
    }

    constexpr float rowHeight = 22.0f;
    constexpr float labelWidth = 55.0f;
    constexpr float rangeTextWidth = 65.0f;
    constexpr float sampleTextWidth = 160.0f;

    const float textWidth =
        rangeTextWidth
        + sampleTextWidth;

    auto area =
        getLocalBounds()
        .toFloat()
        .reduced(8.0f);

    auto barArea =
        juce::Rectangle<float>(
            area.getX() + labelWidth,
            area.getY()
            + draggingZone * rowHeight
            + 5.0f,
            area.getWidth()
            - labelWidth
            - textWidth,
            rowHeight - 10.0f
        );

    auto& zone =
        zones[draggingZone];

    const int velocity =
        xToVelocity(
            event.position.x,
            barArea
        );

    if (draggingLow)
    {
        zone.lowVel =
            static_cast<uint8_t>(
                juce::jlimit(
                    0,
                    static_cast<int>(
                        zone.highVel
                        ),
                    velocity
                )
                );
    }

    if (draggingHigh)
    {
        zone.highVel =
            static_cast<uint8_t>(
                juce::jlimit(
                    static_cast<int>(
                        zone.lowVel
                        ),
                    127,
                    velocity
                )
                );
    }

    repaint();

    if (onZoneRangeChanged)
    {
        onZoneRangeChanged(
            draggingZone,
            zone.lowVel,
            zone.highVel
        );
    }
}

void VelocityZoneMap::mouseUp(
    const juce::MouseEvent&)
{
    draggingZone = -1;
    draggingLow = false;
    draggingHigh = false;
}