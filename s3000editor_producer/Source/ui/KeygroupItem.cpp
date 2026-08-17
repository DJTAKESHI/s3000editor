#include "KeygroupItem.h"
#include "VelocityZoneItem.h"


KeygroupItem::KeygroupItem(
    const Keygroup& kg,
    int idx,
    const std::map<int, SampleHeader>& headers
)
    : keygroup(kg),
    index(idx),
    sampleHeaders(headers)
{
    for (int i = 0; i < keygroup.zones.size(); i++)
    {
        auto* zoneItem =
            new VelocityZoneItem(
                keygroup.zones[i],
                i,
                sampleHeaders
            );


        // ========================================
        // š Sample Header‚ª–³‚­‚Ä‚à“Í‚­Zone‘I‘ð
        // ========================================

        zoneItem->onZoneSelected =
            [this](
                int zoneIndex,
                const VelocityZone& zone
                )
            {
                DBG(
                    "KEYGROUP RECEIVED BASIC ZONE SELECTION"
                );

                DBG(
                    "KEYGROUP INDEX = "
                    + juce::String(index)
                );

                DBG(
                    "ZONE INDEX = "
                    + juce::String(zoneIndex)
                );

                DBG(
                    "ZONE SAMPLE = ["
                    + zone.sampleName
                    + "]"
                );

                if (onBasicZoneSelected)
                {
                    onBasicZoneSelected(
                        index,
                        zoneIndex,
                        zone
                    );
                }
            };


        // ========================================
        // Sample Header‚Ü‚ÅŒ©‚Â‚©‚Á‚½ê‡
        // ========================================

        zoneItem->onSelected =
            [this](
                int zoneIndex,
                const VelocityZone& zone,
                const SampleHeader& sampleHeader
                )
            {
                DBG(
                    "KEYGROUP RECEIVED ZONE SELECTION"
                );

                DBG(
                    "KEYGROUP INDEX = "
                    + juce::String(index)
                );

                DBG(
                    "ZONE INDEX = "
                    + juce::String(zoneIndex)
                );

                DBG(
                    "ZONE SAMPLE = ["
                    + zone.sampleName
                    + "]"
                );

                DBG(
                    "SAMPLE ID = "
                    + juce::String(zone.sampleId)
                );

                DBG(
                    "SAMPLE HEADER = ["
                    + sampleHeader.name
                    + "]"
                );

                if (onZoneSelected)
                {
                    onZoneSelected(
                        index,
                        zoneIndex,
                        zone,
                        sampleHeader
                    );
                }
            };


        addSubItem(zoneItem);
    }
}


void KeygroupItem::paintItem(
    juce::Graphics& g,
    int width,
    int height
)
{
    g.setColour(
        juce::Colours::white
    );


    g.drawText(
        "Keygroup "
        + juce::String(index)
        + "  Note "
        + juce::String(keygroup.lowNote)
        + "-"
        + juce::String(keygroup.highNote),
        5,
        0,
        width - 5,
        height,
        juce::Justification::centredLeft
    );
}

void KeygroupItem::itemClicked(
    const juce::MouseEvent&
)
{
    DBG(
        "KEYGROUP CLICKED INDEX = "
        + juce::String(index)
    );

    if (onSelected)
    {
        onSelected(
            index,
            keygroup
        );
    }
}
