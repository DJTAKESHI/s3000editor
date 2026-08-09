#pragma once

#include <JuceHeader.h>
#include "../s3000/S3000Types.h"
#include "KeygroupItem.h"
#include "ProgramTreeItem.h"


class ProgramTree : public juce::TreeView
{
public:

    ProgramTree();


    void setProgram(
        const Program& program,
        const std::map<int, SampleHeader>& sampleHeaders
    );


    std::function<void(const VelocityZone&, const SampleHeader&)>
        onZoneSelected;


private:


    std::unique_ptr<ProgramTreeItem> rootItem;
};