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

    // 古い Tree を TreeView から外す
    DBG("BEFORE setRootItem(nullptr)");

    setRootItem(nullptr);

    DBG("AFTER setRootItem(nullptr)");

    // 古い root を破棄して、新しい root を作る
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

    // ========================================
// デフォルトでKeygroup 1を選択
// ========================================

    rootItem->setOpen(true);

    if (rootItem->getNumSubItems() > 0
        && !program.keygroups.empty())
    {
        auto* firstKeygroupItem =
            rootItem->getSubItem(0);

        if (firstKeygroupItem != nullptr)
        {
            // Tree上の見た目を選択状態にする
            firstKeygroupItem->setSelected(
                true,
                true
            );

            DBG("DEFAULT SELECTED KEYGROUP 1");

            // Keygroupタブにも値を渡す
            if (onKeygroupSelected)
            {
                DBG(
                    "NOTIFY DEFAULT KEYGROUP SELECTION"
                );

                onKeygroupSelected(
                    0,
                    program.keygroups[0]
                );
            }
        }
    }

}

void ProgramTree::updateZone(
    int keygroupIndex,
    int zoneIndex,
    const VelocityZone& zone
)
{
    if (rootItem == nullptr)
    {
        DBG("UPDATE ZONE: ROOT ITEM IS NULL");
        return;
    }

    rootItem->updateZone(
        keygroupIndex,
        zoneIndex,
        zone
    );
}

void ProgramTree::updateKeygroup(
    int keygroupIndex,
    const Keygroup& keygroup
)
{
    if (rootItem == nullptr)
    {
        DBG("UPDATE KEYGROUP: ROOT ITEM IS NULL");
        return;
    }

    rootItem->updateKeygroup(
        keygroupIndex,
        keygroup
    );
}