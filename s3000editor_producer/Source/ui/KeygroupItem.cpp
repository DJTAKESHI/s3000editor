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

        zoneItem->onSelected =
            [this](
                const VelocityZone& zone,
                const SampleHeader& sampleHeader
                )
            {
                DBG("KEYGROUP RECEIVED ZONE SELECTION");

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
                    onZoneSelected(
                        zone,
                        sampleHeader
                    );
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