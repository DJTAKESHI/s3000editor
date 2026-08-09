#include "ProgramTree.h"
#include "ProgramTreeItem.h"


ProgramTree::ProgramTree()
{
    setRootItemVisible(true);
}


void ProgramTree::setProgram(
    const Program& program,
    const std::map<int, SampleHeader>& sampleHeaders)
{
    DBG("========== SET PROGRAM ==========");

    DBG(
        "sampleHeaders.size = "
        + juce::String((int)sampleHeaders.size())
    );

    // å√Ç¢ Tree Ç TreeView Ç©ÇÁäOÇ∑
    DBG("BEFORE setRootItem(nullptr)");

    setRootItem(nullptr);

    DBG("AFTER setRootItem(nullptr)");

    // å√Ç¢ root Çîjä¸ÇµÇƒÅAêVÇµÇ¢ root ÇçÏÇÈ
    rootItem = std::make_unique<ProgramTreeItem>(
        program,
        sampleHeaders
    );

    rootItem->onZoneSelected =
        [this](
            const VelocityZone& zone,
            const SampleHeader& sampleHeader)
        {
            DBG("PROGRAM TREE RECEIVED ZONE SELECTION");

            if (onZoneSelected)
                onZoneSelected(zone, sampleHeader);
        };

    DBG(
        "NEW ROOT = "
        + juce::String::toHexString(
            reinterpret_cast<juce::pointer_sized_int>(
                rootItem.get()
                )
        )
    );

    setRootItem(rootItem.get());

    DBG("AFTER setRootItem(rootItem)");
}