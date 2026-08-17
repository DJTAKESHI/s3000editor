#pragma once

#include <JuceHeader.h>
#include "../s3000/S3000Types.h"


class KeygroupItem : public juce::TreeViewItem
{
public:

    KeygroupItem(
        const Keygroup& keygroupData,
        int keygroupIndex,
        const std::map<int, SampleHeader>& sampleHeaders
    );


    bool mightContainSubItems() override
    {
        return true;
    }


    void paintItem(
        juce::Graphics& g,
        int width,
        int height
    ) override;

    std::function<void(
        int,
        int,
        const VelocityZone&,
        const SampleHeader&
        )> onZoneSelected;

    std::function<void(
        int,
        const Keygroup&
        )> onSelected;

    void itemClicked(
        const juce::MouseEvent&
    ) override;

    std::function<void(
        int,
        int,
        const VelocityZone&
        )> onBasicZoneSelected;



private:

    const Keygroup& keygroup;

    int index;

    const std::map<int, SampleHeader>& sampleHeaders;
};