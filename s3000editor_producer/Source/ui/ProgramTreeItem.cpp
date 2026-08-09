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

        kgItem->onZoneSelected =
            [this](
                const VelocityZone& zone,
                const SampleHeader& sampleHeader
                )
            {
                //DBG("PROGRAM TREE RECEIVED ZONE SELECTION");

                DBG(
                    "PROGRAM TREE RECEIVED ZONE SELECTION"
                    " THIS="
                    + juce::String::toHexString(
                        reinterpret_cast<juce::pointer_sized_int>(this)
                    )
                );

                if (onZoneSelected)
                    onZoneSelected(
                        zone,
                        sampleHeader
                    );
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