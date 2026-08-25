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

    // ========================================
    // Åö Sample HeaderÇ™ñ≥Ç≠ÇƒÇ‡ZoneëIëÇí ím
    // ========================================

    if (onZoneSelected)
    {
        DBG("NOTIFY ZONE SELECTED");

        onZoneSelected(
            zoneIndex,
            zoneData
        );
    }


    // ========================================
    // sampleIdñ¢âåàÇ»ÇÁ
    // éÊìæçœÇ›Sample HeaderÇ©ÇÁñºëOåüçı
    // ========================================

    if (zoneData.sampleId < 0 &&
        !zoneData.sampleName.trim().isEmpty())
    {
        DBG(
            "sampleHeaders.size = "
            + juce::String(
                (int)sampleHeaders.size()
            )
        );

        const auto target =
            zoneData.sampleName.trim();

        for (const auto& [id, header] :
            sampleHeaders)
        {
            const auto candidate =
                header.name.trim();

            if (candidate == target)
            {
                zoneData.sampleId = id;

                DBG(
                    "RESOLVED SAMPLE ON CLICK -> "
                    + juce::String(id)
                );

                break;
            }
        }
    }


    // ========================================
    // Sample Header lookup
    // ========================================

    auto it =
        sampleHeaders.find(
            zoneData.sampleId
        );

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

        // Sample HeaderÇ‹Ç≈ëµÇ¡ÇƒÇ¢ÇÈèÍçá
        if (onSelected)
        {
            onSelected(
                zoneIndex,
                zoneData,
                it->second
            );
        }
    }
    else
    {
        DBG(
            "SAMPLE HEADER NOT FOUND "
            "- ZONE SELECTION STILL VALID"
        );
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
    g.setColour(
        juce::Colours::white
    );

    juce::String sampleName;

    if (zoneData.sampleName.trim().isEmpty())
    {
        sampleName = "[Empty]";
    }
    else
    {
        sampleName =
            zoneData.sampleName.trim();
    }

    const juce::String text =
        "Zone "
        + juce::String(zoneIndex + 1)
        + "    "
        + sampleName;

    g.drawText(
        text,
        5,
        0,
        width - 10,
        height,
        juce::Justification::centredLeft
    );
}