#pragma once

#include <JuceHeader.h>
#include "../s3000/S3000Types.h"


class VelocityZoneItem :
    public juce::TreeViewItem
{
public:

    VelocityZoneItem(
        const VelocityZone& zone,
        int index,
        const std::map<int, SampleHeader>& sampleHeaders
    );

    ~VelocityZoneItem() override;


    void paintItem(
        juce::Graphics& g,
        int width,
        int height
    ) override;


    bool mightContainSubItems() override
    {
        return false;
    }

    void  itemClicked(
        const juce::MouseEvent& event
    ) override;


    std::function<void(const VelocityZone&, const SampleHeader&)> onSelected;


private:

    VelocityZone zoneData;
    int zoneIndex;

    const std::map<int, SampleHeader>& sampleHeaders;
    

};