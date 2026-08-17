#pragma once

#include <JuceHeader.h>
#include "../s3000/S3000Types.h"


class ProgramTreeItem : public juce::TreeViewItem
{
public:

    //ProgramTreeItem(const juce::String& name);
    ProgramTreeItem(
        const Program& program,
        const std::map<int, SampleHeader>& sampleHeaders
    );

    bool mightContainSubItems() override;

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
        )> onKeygroupSelected;

    std::function<void(
        int,
        int,
        const VelocityZone&
        )> onBasicZoneSelected;





private:

    Program programData;

    const std::map<int, SampleHeader>& sampleHeaders;
};