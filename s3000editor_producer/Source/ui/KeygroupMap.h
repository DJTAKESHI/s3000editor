#pragma once

#include <JuceHeader.h>
#include "../s3000/S3000Types.h"
#include <functional>


class KeygroupMap : public juce::Component
{
public:
    KeygroupMap();
    void setProgram(const Program& program);

    //static constexpr int rowHeight = 32;
    static constexpr int buttonAreaHeight = 44;

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

    //juce::TextButton addKeygroupButton{
    //"+ Add KG"
    //};

    //juce::TextButton deleteKeygroupButton{
    //    "Delete KG"
    //};

    //std::function<void()> onAddKeygroup;
    //std::function<void(int)> onDeleteKeygroup;

private:
    static constexpr int rowHeight = 32;

    //juce::TextButton addKeygroupButton{
    //"+ Add KG"
    //};

    //juce::TextButton deleteKeygroupButton{
    //    "Delete KG"
    //};


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

    //juce::Label titleLabel;

    juce::Rectangle<float> getMapArea() const;

    int keygroupAtPosition(
        juce::Point<float> position) const;
};