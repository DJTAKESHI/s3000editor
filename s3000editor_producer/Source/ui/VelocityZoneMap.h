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

    void mouseDrag(
        const juce::MouseEvent& event
    ) override;

    void mouseUp(
        const juce::MouseEvent& event
    ) override;

    std::function<void(
        int zoneIndex,
        int low,
        int high
        )> onZoneRangeChanged;


private:

    int draggingZone = -1;
    bool draggingLow = false;
    bool draggingHigh = false;
    int selectedZone = 0;

    int xToVelocity(
        float x,
        const juce::Rectangle<float>& barArea
    ) const;

    std::array<VelocityZone, 4> zones;
};