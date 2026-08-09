#include "VelocityZoneItem.h"


VelocityZoneItem::VelocityZoneItem(
    const VelocityZone& zone,
    int index,
    const std::map<int, SampleHeader>& headers
)
    :
    zoneData(zone),
    zoneIndex(index),
    sampleHeaders(headers)
{
    DBG(
        "CREATE ZONE "
        + juce::String(zoneIndex)
        + " SAMPLE=["
        + zoneData.sampleName
        + "]"
        + " ID="
        + juce::String(zoneData.sampleId)
        + " LOW="
        + juce::String(zoneData.lowVel)
        + " HIGH="
        + juce::String(zoneData.highVel)
        + " TUNE="
        + juce::String(zoneData.tune)
        + " LOUD="
        + juce::String(zoneData.loudness)
        + " PAN="
        + juce::String(zoneData.pan);

        DBG(
            "CREATE ZONE "
            + juce::String(zoneIndex)
            + " THIS="
            + juce::String::toHexString(
                reinterpret_cast<uintptr_t>(this)
            )
            + " SAMPLE=["
            + zoneData.sampleName
            + "] ID="
            + juce::String(zoneData.sampleId)
        );

    );
}

void VelocityZoneItem::itemClicked(
    const juce::MouseEvent&
)
{
    DBG("========== ITEM CLICKED ==========");

    DBG(
        "THIS="
        + juce::String::toHexString(
            reinterpret_cast<uintptr_t>(this)
        )
    );

    DBG(
        "ZONE INDEX = "
        + juce::String(zoneIndex)
    );

    DBG(
        "SAMPLE NAME = ["
        + zoneData.sampleName
        + "]"
    );

    DBG(
        "SAMPLE ID = "
        + juce::String(zoneData.sampleId)
    );

    auto it =
        sampleHeaders.find(zoneData.sampleId);

    // ===== Sample Header lookup =====
    if (it != sampleHeaders.end())
    {
        DBG("FOUND SAMPLE HEADER");

        DBG(
            "Name = "
            + it->second.name
        );

        DBG(
            "Pitch = "
            + juce::String(
                it->second.originalPitch
            )
        );

        DBG(
            "Length = "
            + juce::String(
                it->second.length
            )
        );

        // Zone + SampleHeader ‚ðe‚Ö“n‚·
        if (onSelected)
            onSelected(zoneData, it->second);
    }
    else
    {
        DBG("SAMPLE HEADER NOT FOUND");
    }
}

VelocityZoneItem::~VelocityZoneItem()
{
    DBG(
        "DESTROY ZONE "
        + juce::String(zoneIndex)
        + " THIS="
        + juce::String::toHexString(
            reinterpret_cast<juce::pointer_sized_int>(this)
        )
        + " SAMPLE ID="
        + juce::String(zoneData.sampleId)
    );
}


void VelocityZoneItem::paintItem(
    juce::Graphics& g,
    int width,
    int height)
{
    g.setColour(juce::Colours::white);

    juce::String text;

    text = "Zone "
        + juce::String(zoneIndex)
        + "   ";

    // Sample
    if (zoneData.sampleName.trim().isEmpty())
    {
        text += "[Empty]";
    }
    else
    {
        text += zoneData.sampleName.trim();
    }

    // Velocity
    text += "   Vel:"
        + juce::String(zoneData.lowVel)
        + "-"
        + juce::String(zoneData.highVel);

    // Tune
    text += "   Tune:"
        + juce::String(zoneData.tune);

    // Loudness
    text += "   Loud:"
        + juce::String(zoneData.loudness);

    // Pan
    text += "   Pan:"
        + juce::String(zoneData.pan);

    // Play mode
    text += "   Play:"
        + juce::String(static_cast<int>(zoneData.playMode));

    g.drawText(
        text,
        5,
        0,
        width - 10,
        height,
        juce::Justification::centredLeft
    );
}