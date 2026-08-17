#include "ProgramTreeItem.h"
#include "KeygroupItem.h"


ProgramTreeItem::ProgramTreeItem(
    const Program& program,
    const std::map<int, SampleHeader>& sampleHeaders
)
    : programData(program),
    sampleHeaders(sampleHeaders)
{
    for (int i = 0; i < program.keygroups.size(); i++)
    {
        auto* kgItem =
            new KeygroupItem(
                program.keygroups[i],
                i,
                sampleHeaders
            );

        // ========================================
        // Sample Header‚ª‚È‚­‚Ä‚à“Í‚­Zone‘I‘ð
        // ========================================

        kgItem->onBasicZoneSelected =
            [this](
                int keygroupIndex,
                int zoneIndex,
                const VelocityZone& zone
                )
            {
                DBG(
                    "PROGRAM ROOT RECEIVED BASIC ZONE SELECTION"
                );

                DBG(
                    "KEYGROUP INDEX = "
                    + juce::String(keygroupIndex)
                );

                DBG(
                    "ZONE INDEX = "
                    + juce::String(zoneIndex)
                );

                if (onBasicZoneSelected)
                {
                    onBasicZoneSelected(
                        keygroupIndex,
                        zoneIndex,
                        zone
                    );
                }
            };


        // ========================================
        // Sample Header‚Ü‚ÅŒ©‚Â‚©‚Á‚½ê‡
        // ========================================

        kgItem->onZoneSelected =
            [this](
                int keygroupIndex,
                int zoneIndex,
                const VelocityZone& zone,
                const SampleHeader& sampleHeader
                )
            {
                DBG(
                    "PROGRAM TREE RECEIVED ZONE SELECTION"
                    " THIS="
                    + juce::String::toHexString(
                        reinterpret_cast<
                        juce::pointer_sized_int
                        >(this)
                    )
                );

                if (onZoneSelected)
                {
                    onZoneSelected(
                        keygroupIndex,
                        zoneIndex,
                        zone,
                        sampleHeader
                    );
                }
            };


        // ========================================
        // Keygroup‘I‘ð
        // ========================================

        kgItem->onSelected =
            [this](
                int keygroupIndex,
                const Keygroup& keygroup
                )
            {
                DBG(
                    "PROGRAM ROOT RECEIVED KEYGROUP SELECTION "
                    + juce::String(keygroupIndex)
                );

                DBG(
                    "ROOT THIS = "
                    + juce::String::toHexString(
                        reinterpret_cast<
                        juce::pointer_sized_int
                        >(this)
                    )
                );

                DBG(
                    "HAS onKeygroupSelected = "
                    + juce::String(
                        onKeygroupSelected
                        ? "YES"
                        : "NO"
                    )
                );

                if (onKeygroupSelected)
                {
                    DBG(
                        "CALLING ROOT onKeygroupSelected"
                    );

                    onKeygroupSelected(
                        keygroupIndex,
                        keygroup
                    );
                }
            };


        addSubItem(kgItem);
    }
}

bool ProgramTreeItem::mightContainSubItems()
{
    return !programData.keygroups.empty();
}


void ProgramTreeItem::paintItem(
    juce::Graphics& g,
    int width,
    int height
)
{
    g.drawText(
        programData.name,
        5,
        0,
        width,
        height,
        juce::Justification::centredLeft
    );
}