#pragma once

#include <JuceHeader.h>
#include "../s3000/S3000Types.h"

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
    juce::TextEditor filterFreqEditor;

    juce::Label filterKeyFollowLabel;
    juce::TextEditor filterKeyFollowEditor;

    juce::Label velocityToFreqLabel;
    juce::TextEditor velocityToFreqEditor;

    juce::Label pressureToFreqLabel;
    juce::TextEditor pressureToFreqEditor;

    juce::Label envelopeToFreqLabel;
    juce::TextEditor envelopeToFreqEditor;

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


};