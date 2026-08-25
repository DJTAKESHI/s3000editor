#pragma once

#include <JuceHeader.h>
#include "../s3000/S3000Types.h"

class KeygroupMap : public juce::Component
{
public:
    void setProgram(const Program& program);

    // Keygroupをクリックしたとき
    std::function<void(int)>
        onKeygroupSelected;

    // KeygroupMap.h

    std::function<void(
        int keygroupIndex,
        int lowNote,
        int highNote)>
        onRangeChangeFinished;

    // Low / High Noteを変更したとき
    std::function<void(
        int keygroupIndex,
        int lowNote,
        int highNote)>
        onRangeChanged;

    void paint(
        juce::Graphics& g) override;

    void mouseDown(
        const juce::MouseEvent& event) override;

    void mouseDrag(
        const juce::MouseEvent& event) override;

    void mouseUp(
        const juce::MouseEvent& event) override;

    void resized() override;

private:
    static constexpr int rowHeight = 32;

    enum class DragMode
    {
        none,
        lowNote,
        highNote
    };

    Program currentProgram;

    int selectedKeygroup = -1;

    DragMode dragMode =
        DragMode::none;

    static constexpr float handleWidth =
        14.0f;

    int noteFromX(
        float x) const;

    juce::Rectangle<float>
        getBarBounds(
            int keygroupIndex) const;

    float noteToX(
        int note) const;

    int keygroupAtPosition(
        juce::Point<float> position) const;
};