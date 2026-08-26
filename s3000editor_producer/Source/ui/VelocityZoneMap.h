#pragma once

#include <JuceHeader.h>
#include "../s3000/S3000Types.h"

class VelocityZoneMap
    : public juce::Component
{
public:
    void setZones(
        const std::array<VelocityZone, 4>& zones
    );

    void paint(
        juce::Graphics& g
    ) override;

    void mouseDown(
        const juce::MouseEvent& event
    ) override;

    std::function<void(int)>
        onZoneSelected;

    void setSelectedZone(int index);

private:
    int selectedZone = 0;
    std::array<VelocityZone, 4> zones;
};