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
            int keygroupIndex,
            int zoneIndex,
            const VelocityZone& zone,
            const SampleHeader& sampleHeader)
        {
            DBG("PROGRAM TREE RECEIVED ZONE SELECTION");

            DBG(
                "KEYGROUP INDEX = "
                + juce::String(keygroupIndex)
            );

            DBG(
                "ZONE INDEX = "
                + juce::String(zoneIndex)
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

    rootItem->onBasicZoneSelected =
        [this](
            int keygroupIndex,
            int zoneIndex,
            const VelocityZone& zone
            )
        {
            DBG(
                "PROGRAM TREE RECEIVED BASIC ZONE SELECTION"
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


    rootItem->onKeygroupSelected =
        [this](
            int keygroupIndex,
            const Keygroup& keygroup
            )
        {
            DBG(
                "PROGRAM TREE RECEIVED KEYGROUP SELECTION "
                + juce::String(keygroupIndex)
            );

            if (onKeygroupSelected)
            {
                onKeygroupSelected(
                    keygroupIndex,
                    keygroup
                );
            }
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