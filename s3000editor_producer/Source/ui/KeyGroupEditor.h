#pragma once

#include <JuceHeader.h>
#include "../s3000/S3000Types.h"
#include "EnvelopeEditor.h"
#include "Envelope2Editor.h"

class KeyGroupEditor : public juce::Component
{
public:
    KeyGroupEditor();

    void resized() override;

    void setKeygroup(
        const Keygroup& keygroup,
        int keygroupIndex
    );

    std::function<void(
        int,
        const Keygroup&
        )> onKeygroupChanged;

    void paint(juce::Graphics& g) override;

    void setResonance(int value);

    std::function<void(
        int,
        int
        )> onResonanceChanged;

    void setFilterFreq(int value);

    void setFilterKeyFollow(int value);

    std::function<void(
        int,
        int
        )> onFilterKeyFollowChanged;

    std::function<void(
        int,
        int
        )> onFilterFreqChanged;


    std::function<void(
        int,
        int
        )> onEnv2R1Changed;

    std::function<void(int, int)> onEnv2L1Changed;

    std::function<void(int, int)> onEnv2R2Changed;


private:
    juce::Label titleLabel;

    juce::Label lowNoteLabel;
    juce::TextEditor lowNoteEditor;

    juce::Label highNoteLabel;
    juce::TextEditor highNoteEditor;

    Keygroup currentKeygroup;
    int currentKeygroupIndex = -1;

    juce::Label tuneLabel;
    juce::TextEditor tuneEditor;

    juce::Label filterFreqLabel;
    juce::Slider filterFreqKnob;

    juce::Label filterKeyFollowLabel;
    juce::Slider filterKeyFollowKnob;

    juce::Label velocityToFreqLabel;
    juce::Slider velocityToFreqKnob;

    juce::Label pressureToFreqLabel;
    juce::Slider pressureToFreqKnob;

    juce::Label envelopeToFreqLabel;
    juce::Slider envelopeToFreqKnob;

    juce::Label resonanceLabel;
    juce::Slider resonanceKnob;

    juce::Label env1AttackLabel;
    juce::TextEditor env1AttackEditor;

    juce::Label env1DecayLabel;
    juce::TextEditor env1DecayEditor;

    juce::Label env1SustainLabel;
    juce::TextEditor env1SustainEditor;

    juce::Label env1ReleaseLabel;
    juce::TextEditor env1ReleaseEditor;

    // ===== ENV2 =====
    juce::Label env2R1Label;
    juce::TextEditor env2R1Editor;

    juce::Label env2L1Label;
    juce::TextEditor env2L1Editor;

    juce::Label env2R2Label;
    juce::TextEditor env2R2Editor;

    juce::Label env2L2Label;
    juce::TextEditor env2L2Editor;

    juce::Label env2R3Label;
    juce::TextEditor env2R3Editor;

    juce::Label env2L3Label;
    juce::TextEditor env2L3Editor;

    juce::Label env2R4Label;
    juce::TextEditor env2R4Editor;

    juce::Label env2L4Label;
    juce::TextEditor env2L4Editor;

    //juce::Label env2AttackLabel;
    //juce::TextEditor env2AttackEditor;

    //juce::Label env2DecayLabel;
    //juce::TextEditor env2DecayEditor;

    //juce::Label env2SustainLabel;
    //juce::TextEditor env2SustainEditor;

    //juce::Label env2ReleaseLabel;
    //juce::TextEditor env2ReleaseEditor;

    juce::Label env2VelAttackLabel;
    juce::TextEditor env2VelAttackEditor;

    juce::Label env2VelReleaseLabel;
    juce::TextEditor env2VelReleaseEditor;

    juce::Label env2NoteOffReleaseLabel;
    juce::TextEditor env2NoteOffReleaseEditor;

    juce::Label env2KeyTrackingLabel;
    juce::TextEditor env2KeyTrackingEditor;

    juce::Label velocityEnv2Label;
    juce::TextEditor velocityEnv2Editor;

    juce::Label ePtchLabel;
    juce::TextEditor ePtchEditor;

    juce::Label velocityXFadeLabel;
    juce::TextEditor velocityXFadeEditor;

    juce::Label velocityZonesLabel;
    juce::TextEditor velocityZonesEditor;

    juce::Label leftKeyXFadeLabel;
    juce::TextEditor leftKeyXFadeEditor;

    juce::Label rightKeyXFadeLabel;
    juce::TextEditor rightKeyXFadeEditor;

    juce::Label env1SectionLabel;
    juce::Label env2SectionLabel;

    juce::Label modulationSectionLabel;
    juce::Label keyXFadeSectionLabel;

    juce::Label keyRangeSectionLabel;
    juce::Label filterSectionLabel;

    EnvelopeEditor env1Editor;
    Envelope2Editor env2Editor;

   


};