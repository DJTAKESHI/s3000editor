#include "KeyGroupEditor.h"

KeyGroupEditor::KeyGroupEditor()
{
    titleLabel.setText(
        "Keygroup",
        juce::dontSendNotification
    );

    titleLabel.setFont(
        juce::Font(18.0f, juce::Font::bold)
    );

    lowNoteLabel.setText(
        "Low Note",
        juce::dontSendNotification
    );

    highNoteLabel.setText(
        "High Note",
        juce::dontSendNotification
    );

    tuneLabel.setText(
        "Tune",
        juce::dontSendNotification
    );

    addAndMakeVisible(tuneLabel);
    addAndMakeVisible(tuneEditor);

    addAndMakeVisible(titleLabel);

    addAndMakeVisible(lowNoteLabel);
    addAndMakeVisible(lowNoteEditor);

    addAndMakeVisible(highNoteLabel);
    addAndMakeVisible(highNoteEditor);


    lowNoteEditor.onFocusLost = [this]()
        {
            const int value =
                lowNoteEditor.getText().getIntValue();

            // S3000XLの範囲
            if (value < 21 || value > 127)
            {
                lowNoteEditor.setText(
                    juce::String(currentKeygroup.lowNote),
                    false
                );
                return;
            }

            // Low Note が High Note を超えない
            if (value > currentKeygroup.highNote)
            {
                lowNoteEditor.setText(
                    juce::String(currentKeygroup.lowNote),
                    false
                );
                return;
            }

            // ★変更がなければ終了
            if (value == currentKeygroup.lowNote)
                return;

            // ★比較した後で代入
            currentKeygroup.lowNote = value;

            DBG(
                "LOW NOTE EDITOR CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
            {
                DBG("CALL onKeygroupChanged");

                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };


    highNoteEditor.onFocusLost = [this]()
        {
            const int value =
                highNoteEditor.getText().getIntValue();

            if (value < 21 || value > 127)
            {
                highNoteEditor.setText(
                    juce::String(currentKeygroup.highNote),
                    false
                );
                return;
            }

            // High Note が Low Note より小さくならない
            if (value < currentKeygroup.lowNote)
            {
                highNoteEditor.setText(
                    juce::String(currentKeygroup.highNote),
                    false
                );
                return;
            }

            // ★変更なしなら終了
            if (value == currentKeygroup.highNote)
                return;

            // ★比較後に代入
            currentKeygroup.highNote = value;

            DBG(
                "HIGH NOTE EDITOR CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
            {
                DBG("CALL onKeygroupChanged");

                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };

    addAndMakeVisible(filterFreqLabel);
    addAndMakeVisible(filterFreqEditor);

    addAndMakeVisible(filterKeyFollowLabel);
    addAndMakeVisible(filterKeyFollowEditor);

    env1AttackLabel.setText("Attack", juce::dontSendNotification);
    env1DecayLabel.setText("Decay", juce::dontSendNotification);
    env1SustainLabel.setText("Sustain", juce::dontSendNotification);
    env1ReleaseLabel.setText("Release", juce::dontSendNotification);

    addAndMakeVisible(env1SectionLabel);
    env1SectionLabel.setText(
        "ENV1",
        juce::dontSendNotification
    );

    env1SectionLabel.setFont(
        juce::Font(15.0f, juce::Font::bold)
    );

    addAndMakeVisible(env2SectionLabel);
    env2SectionLabel.setText(
        "ENV2",
        juce::dontSendNotification
    );

    env2SectionLabel.setFont(
        juce::Font(15.0f, juce::Font::bold)
    );

    addAndMakeVisible(env1AttackLabel);
    addAndMakeVisible(env1AttackEditor);

    addAndMakeVisible(env1DecayLabel);
    addAndMakeVisible(env1DecayEditor);

    addAndMakeVisible(env1SustainLabel);
    addAndMakeVisible(env1SustainEditor);

    addAndMakeVisible(env1ReleaseLabel);
    addAndMakeVisible(env1ReleaseEditor);

    //env2AttackLabel.setText("ENV2 Attack", juce::dontSendNotification);
    //env2DecayLabel.setText("ENV2 Decay", juce::dontSendNotification);
    //env2SustainLabel.setText("ENV2 Sustain", juce::dontSendNotification);
    //env2ReleaseLabel.setText("ENV2 Release", juce::dontSendNotification);

    env2R1Label.setText(
        "R1 (Attack)",
        juce::dontSendNotification
    );

    env2L1Label.setText(
        "L1 (Attack)",
        juce::dontSendNotification
    );

    env2R2Label.setText(
        "R2 (Phase 2)",
        juce::dontSendNotification
    );

    env2L2Label.setText(
        "L2 (Phase 2)",
        juce::dontSendNotification
    );

    env2R3Label.setText(
        "R3 (Decay)",
        juce::dontSendNotification
    );

    env2L3Label.setText(
        "L3 (Sustain)",
        juce::dontSendNotification
    );

    env2R4Label.setText(
        "R4 (Release)",
        juce::dontSendNotification
    );

    env2L4Label.setText(
        "L4 (Final)",
        juce::dontSendNotification
    );

    addAndMakeVisible(keyRangeSectionLabel);
    keyRangeSectionLabel.setText(
        "KEY RANGE",
        juce::dontSendNotification
    );

    addAndMakeVisible(filterSectionLabel);
    filterSectionLabel.setText(
        "FILTER",
        juce::dontSendNotification
    );

    keyRangeSectionLabel.setFont(
        juce::Font(15.0f, juce::Font::bold)
    );

    filterSectionLabel.setFont(
        juce::Font(15.0f, juce::Font::bold)
    );

    addAndMakeVisible(env2R1Label);
    addAndMakeVisible(env2R1Editor);

    addAndMakeVisible(env2L1Label);
    addAndMakeVisible(env2L1Editor);

    addAndMakeVisible(env2R2Label);
    addAndMakeVisible(env2R2Editor);

    addAndMakeVisible(env2L2Label);
    addAndMakeVisible(env2L2Editor);

    addAndMakeVisible(env2R3Label);
    addAndMakeVisible(env2R3Editor);

    addAndMakeVisible(env2L3Label);
    addAndMakeVisible(env2L3Editor);

    addAndMakeVisible(env2R4Label);
    addAndMakeVisible(env2R4Editor);

    addAndMakeVisible(env2L4Label);
    addAndMakeVisible(env2L4Editor);


    filterFreqLabel.setText(
        "Filter Freq",
        juce::dontSendNotification
    );

    filterKeyFollowLabel.setText(
        "Key Follow",
        juce::dontSendNotification
    );

    velocityToFreqLabel.setText(
        "Velocity -> Freq",
        juce::dontSendNotification
    );

    pressureToFreqLabel.setText(
        "Pressure -> Freq",
        juce::dontSendNotification
    );

    envelopeToFreqLabel.setText(
        "Envelope -> Freq",
        juce::dontSendNotification
    );

    velocityEnv2Label.setText(
        "Velocity -> ENV2",
        juce::dontSendNotification
    );

    velocityZonesLabel.setText(
        "Velocity Zones",
        juce::dontSendNotification
    );

    leftKeyXFadeLabel.setText(
        "Left Key XFade",
        juce::dontSendNotification
    );

    rightKeyXFadeLabel.setText(
        "Right Key XFade",
        juce::dontSendNotification
    );

    addAndMakeVisible(modulationSectionLabel);
    modulationSectionLabel.setText(
        "MODULATION",
        juce::dontSendNotification
    );

    modulationSectionLabel.setFont(
        juce::Font(15.0f, juce::Font::bold)
    );

    keyXFadeSectionLabel.setFont(
        juce::Font(15.0f, juce::Font::bold)
    );


    addAndMakeVisible(keyXFadeSectionLabel);
    keyXFadeSectionLabel.setText(
        "KEY XFADE",
        juce::dontSendNotification
    );

    velocityEnv2Label.setText(
        "Vel -> ENV2",
        juce::dontSendNotification
    );

    ePtchLabel.setText(
        "Env -> Pitch",
        juce::dontSendNotification
    );

    velocityXFadeLabel.setText(
        "Velocity XFade",
        juce::dontSendNotification
    );

    velocityZonesLabel.setText(
        "Velocity Zones",
        juce::dontSendNotification
    );

    leftKeyXFadeLabel.setText(
        "Left",
        juce::dontSendNotification
    );

    rightKeyXFadeLabel.setText(
        "Right",
        juce::dontSendNotification
    );

    addAndMakeVisible(velocityZonesLabel);
    addAndMakeVisible(velocityZonesEditor);

    addAndMakeVisible(leftKeyXFadeLabel);
    addAndMakeVisible(leftKeyXFadeEditor);

    addAndMakeVisible(rightKeyXFadeLabel);
    addAndMakeVisible(rightKeyXFadeEditor);


    addAndMakeVisible(velocityEnv2Label);
    addAndMakeVisible(velocityEnv2Editor);

    env1AttackEditor.onFocusLost = [this]()
        {
            const int value =
                env1AttackEditor.getText().getIntValue();

            if (value < 0 || value > 127)
            {
                env1AttackEditor.setText(
                    juce::String(currentKeygroup.env1.attack),
                    false
                );
                return;
            }

            if (value == currentKeygroup.env1.attack)
                return;

            currentKeygroup.env1.attack = value;

            DBG(
                "ENV1 ATTACK CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
            {
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };

    env1DecayEditor.onFocusLost = [this]()
        {
            const int value = env1DecayEditor.getText().getIntValue();

            if (value < 0 || value > 127)
                return;

            if (value == currentKeygroup.env1.decay)
                return;

            currentKeygroup.env1.decay = value;

            if (onKeygroupChanged)
                onKeygroupChanged(currentKeygroupIndex, currentKeygroup);
        };

    env1SustainEditor.onFocusLost = [this]()
        {
            const int value =
                env1SustainEditor.getText().getIntValue();

            if (value < 0 || value > 99)
            {
                env1SustainEditor.setText(
                    juce::String(currentKeygroup.env1.sustain),
                    false
                );
                return;
            }

            if (value == currentKeygroup.env1.sustain)
                return;

            currentKeygroup.env1.sustain = value;

            DBG(
                "ENV1 SUSTAIN CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
            {
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };

    env1ReleaseEditor.onFocusLost = [this]()
        {
            const int value =
                env1ReleaseEditor.getText().getIntValue();

            if (value < 0 || value > 99)
            {
                env1ReleaseEditor.setText(
                    juce::String(currentKeygroup.env1.release),
                    false
                );
                return;
            }

            if (value == currentKeygroup.env1.release)
                return;

            currentKeygroup.env1.release = value;

            DBG(
                "ENV1 RELEASE CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
            {
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };



    addAndMakeVisible(velocityToFreqLabel);
    addAndMakeVisible(velocityToFreqEditor);

    addAndMakeVisible(pressureToFreqLabel);
    addAndMakeVisible(pressureToFreqEditor);

    addAndMakeVisible(envelopeToFreqLabel);
    addAndMakeVisible(envelopeToFreqEditor);


    filterFreqEditor.onFocusLost = [this]()
        {
            const int value =
                filterFreqEditor.getText().getIntValue();

            if (value < 0 || value > 127)
            {
                filterFreqEditor.setText(
                    juce::String(currentKeygroup.filter.freq),
                    false
                );
                return;
            }

            // ★ 値が同じなら送信しない
            if (value == currentKeygroup.filter.freq)
                return;

            currentKeygroup.filter.freq = value;

            DBG(
                "FILTER FREQ CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
            {
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };

    filterKeyFollowEditor.onFocusLost = [this]()
        {
            const int value =
                filterKeyFollowEditor.getText().getIntValue();

            // ひとまず安全な範囲
            if (value < 0 || value > 127)
            {
                filterKeyFollowEditor.setText(
                    juce::String(currentKeygroup.filter.keyFollow),
                    false
                );
                return;
            }

            // 値が変わっていなければ何もしない
            if (value == currentKeygroup.filter.keyFollow)
                return;

            currentKeygroup.filter.keyFollow = value;

            DBG(
                "FILTER KEY FOLLOW CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
            {
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };

    tuneEditor.onFocusLost = [this]()
        {
            const int value =
                tuneEditor.getText().getIntValue();

            if (value < -50 || value > 50)
            {
                tuneEditor.setText(
                    juce::String(currentKeygroup.tune),
                    false
                );
                return;
            }

            if (value == currentKeygroup.tune)
                return;

            currentKeygroup.tune = value;

            DBG(
                "KEYGROUP TUNE CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
            {
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };

    velocityToFreqEditor.onFocusLost = [this]()
        {
            const int value =
                velocityToFreqEditor.getText().getIntValue();

            if (value < -128 || value > 127)
            {
                velocityToFreqEditor.setText(
                    juce::String(currentKeygroup.filter.velocityToFreq),
                    false
                );
                return;
            }

            if (value == currentKeygroup.filter.velocityToFreq)
                return;

            currentKeygroup.filter.velocityToFreq = value;

            DBG(
                "VELOCITY TO FREQ CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
            {
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };

    pressureToFreqEditor.onFocusLost = [this]()
        {
            const int value =
                pressureToFreqEditor.getText().getIntValue();

            if (value < -128 || value > 127)
            {
                pressureToFreqEditor.setText(
                    juce::String(currentKeygroup.filter.pressureToFreq),
                    false
                );
                return;
            }

            if (value == currentKeygroup.filter.pressureToFreq)
                return;

            currentKeygroup.filter.pressureToFreq = value;

            DBG(
                "PRESSURE TO FREQ CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
            {
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };

    envelopeToFreqEditor.onFocusLost = [this]()
        {
            const int value =
                envelopeToFreqEditor.getText().getIntValue();

            if (value < -128 || value > 127)
            {
                envelopeToFreqEditor.setText(
                    juce::String(currentKeygroup.filter.envelopeToFreq),
                    false
                );
                return;
            }

            if (value == currentKeygroup.filter.envelopeToFreq)
                return;

            currentKeygroup.filter.envelopeToFreq = value;

            DBG(
                "ENVELOPE TO FREQ CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
            {
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };

    /*env2AttackEditor.onFocusLost = [this]()
        {
            const int value = env2AttackEditor.getText().getIntValue();

            if (value < 0 || value > 127)
                return;

            if (value == currentKeygroup.env2.attack)
                return;

            currentKeygroup.env2.attack = value;

            if (onKeygroupChanged)
                onKeygroupChanged(currentKeygroupIndex, currentKeygroup);
        };

    env2DecayEditor.onFocusLost = [this]()
        {
            const int value = env2DecayEditor.getText().getIntValue();

            if (value < 0 || value > 127)
                return;

            if (value == currentKeygroup.env2.decay)
                return;

            currentKeygroup.env2.decay = value;

            if (onKeygroupChanged)
                onKeygroupChanged(currentKeygroupIndex, currentKeygroup);
        };

    env2SustainEditor.onFocusLost = [this]()
        {
            const int value = env2SustainEditor.getText().getIntValue();

            if (value < 0 || value > 127)
                return;

            if (value == currentKeygroup.env2.sustain)
                return;

            currentKeygroup.env2.sustain = value;

            if (onKeygroupChanged)
                onKeygroupChanged(currentKeygroupIndex, currentKeygroup);
        };

    env2ReleaseEditor.onFocusLost = [this]()
        {
            const int value = env2ReleaseEditor.getText().getIntValue();

            if (value < 0 || value > 127)
                return;

            if (value == currentKeygroup.env2.release)
                return;

            currentKeygroup.env2.release = value;

            if (onKeygroupChanged)
                onKeygroupChanged(currentKeygroupIndex, currentKeygroup);
        };*/

    env2R1Editor.onFocusLost = [this]()
        {
            const int value = env2R1Editor.getText().getIntValue();

            if (value < 0 || value > 99)
                return;

            if (value == currentKeygroup.env2.r1)
                return;

            currentKeygroup.env2.r1 = value;

            if (onKeygroupChanged)
                onKeygroupChanged(currentKeygroupIndex, currentKeygroup);
        };

    env2L1Editor.onFocusLost = [this]()
        {
            const int value = env2L1Editor.getText().getIntValue();

            if (value < 0 || value > 99)
                return;

            if (value == currentKeygroup.env2.l1)
                return;

            currentKeygroup.env2.l1 = value;

            if (onKeygroupChanged)
                onKeygroupChanged(currentKeygroupIndex, currentKeygroup);
        };

    env2R2Editor.onFocusLost = [this]()
        {
            const int value = env2R2Editor.getText().getIntValue();

            if (value < 0 || value > 99)
                return;

            if (value == currentKeygroup.env2.r2)
                return;

            currentKeygroup.env2.r2 = value;

            if (onKeygroupChanged)
                onKeygroupChanged(currentKeygroupIndex, currentKeygroup);
        };

    env2L2Editor.onFocusLost = [this]()
        {
            const int value = env2L2Editor.getText().getIntValue();

            if (value < 0 || value > 99)
                return;

            if (value == currentKeygroup.env2.l2)
                return;

            currentKeygroup.env2.l2 = value;

            if (onKeygroupChanged)
                onKeygroupChanged(currentKeygroupIndex, currentKeygroup);
        };

    env2R3Editor.onFocusLost = [this]()
        {
            const int value = env2R3Editor.getText().getIntValue();

            if (value < 0 || value > 99)
                return;

            if (value == currentKeygroup.env2.r3)
                return;

            currentKeygroup.env2.r3 = value;

            if (onKeygroupChanged)
                onKeygroupChanged(currentKeygroupIndex, currentKeygroup);
        };

    env2L3Editor.onFocusLost = [this]()
        {
            const int value = env2L3Editor.getText().getIntValue();

            if (value < 0 || value > 99)
                return;

            if (value == currentKeygroup.env2.l3)
                return;

            currentKeygroup.env2.l3 = value;

            if (onKeygroupChanged)
                onKeygroupChanged(currentKeygroupIndex, currentKeygroup);
        };

    env2R4Editor.onFocusLost = [this]()
        {
            const int value = env2R4Editor.getText().getIntValue();

            if (value < 0 || value > 99)
                return;

            if (value == currentKeygroup.env2.r4)
                return;

            currentKeygroup.env2.r4 = value;

            if (onKeygroupChanged)
                onKeygroupChanged(currentKeygroupIndex, currentKeygroup);
        };

    env2L4Editor.onFocusLost = [this]()
        {
            const int value = env2L4Editor.getText().getIntValue();

            if (value < 0 || value > 99)
                return;

            if (value == currentKeygroup.env2.l4)
                return;

            currentKeygroup.env2.l4 = value;

            if (onKeygroupChanged)
                onKeygroupChanged(currentKeygroupIndex, currentKeygroup);
        };


    env2VelAttackEditor.onFocusLost = [this]()
        {
            const int value =
                env2VelAttackEditor.getText().getIntValue();

            if (value < -128 || value > 127)
                return;

            if (value == currentKeygroup.env2.velAttack)
                return;

            currentKeygroup.env2.velAttack = value;

            DBG(
                "ENV2 VEL ATTACK CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
            {
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };

    env2VelReleaseEditor.onFocusLost = [this]()
        {
            const int value =
                env2VelReleaseEditor.getText().getIntValue();

            if (value < -128 || value > 127)
                return;

            if (value == currentKeygroup.env2.velRelease)
                return;

            currentKeygroup.env2.velRelease = value;

            if (onKeygroupChanged)
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
        };

    env2NoteOffReleaseEditor.onFocusLost = [this]()
        {
            const int value =
                env2NoteOffReleaseEditor.getText().getIntValue();

            if (value < -128 || value > 127)
                return;

            if (value == currentKeygroup.env2.noteOffRelease)
                return;

            currentKeygroup.env2.noteOffRelease = value;

            if (onKeygroupChanged)
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
        };

    env2KeyTrackingEditor.onFocusLost = [this]()
        {
            const int value =
                env2KeyTrackingEditor.getText().getIntValue();

            if (value < -128 || value > 127)
                return;

            if (value == currentKeygroup.env2.keyTracking)
                return;

            currentKeygroup.env2.keyTracking = value;

            if (onKeygroupChanged)
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
        };

    velocityEnv2Editor.onFocusLost = [this]()
        {
            const int value =
                velocityEnv2Editor.getText().getIntValue();

            if (value < -128 || value > 127)
                return;

            if (value == currentKeygroup.velocity.vEnv2)
                return;

            currentKeygroup.velocity.vEnv2 = value;

            DBG(
                "VELOCITY TO ENV2 CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
            {
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };

    ePtchEditor.onFocusLost = [this]()
        {
            const int value =
                ePtchEditor.getText().getIntValue();

            if (value < 0 || value > 127)
                return;

            if (value == currentKeygroup.velocity.ePtch)
                return;

            currentKeygroup.velocity.ePtch = value;

            DBG(
                "E TO PITCH CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
            {
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };

    velocityXFadeEditor.onFocusLost = [this]()
        {
            const int value =
                velocityXFadeEditor.getText().getIntValue();

            if (value < 0 || value > 127)
                return;

            if (value == currentKeygroup.velocity.vxFade)
                return;

            currentKeygroup.velocity.vxFade = value;

            DBG(
                "VELOCITY XFADE CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
            {
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };

    velocityZonesEditor.onFocusLost = [this]()
        {
            const int value =
                velocityZonesEditor.getText().getIntValue();

            if (value < 0 || value > 127)
                return;

            if (value == currentKeygroup.velocity.vZones)
                return;

            currentKeygroup.velocity.vZones = value;

            if (onKeygroupChanged)
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
        };


    leftKeyXFadeEditor.onFocusLost = [this]()
        {
            const int value =
                leftKeyXFadeEditor.getText().getIntValue();

            if (value < 0 || value > 255)
                return;

            if (value == currentKeygroup.velocity.lkxf)
                return;

            currentKeygroup.velocity.lkxf =
                static_cast<uint8_t>(value);

            DBG(
                "LEFT KEY XFADE CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
        };




    rightKeyXFadeEditor.onFocusLost = [this]()
        {
            const int value =
                rightKeyXFadeEditor.getText().getIntValue();

            
            if (value < 0 || value > 255)
            {
                rightKeyXFadeEditor.setText(
                    juce::String(
                        (int)currentKeygroup.velocity.rkxf
                    ),
                    false
                );
                return;
            }

            
            if (value == currentKeygroup.velocity.rkxf)
                return;

            currentKeygroup.velocity.rkxf =
                static_cast<uint8_t>(value);

            DBG(
                "RIGHT KEY XFADE CHANGED = "
                + juce::String(value)
            );

            if (onKeygroupChanged)
            {
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };


    auto setupEnv2Editor =
        [this](
            juce::TextEditor& editor,
            int Envelope2::* member
            )
        {
            editor.onFocusLost =
                [this, &editor, member]()
                {
                    const int value =
                        editor.getText().getIntValue();

                    if (value < 0 || value > 99)
                    {
                        editor.setText(
                            juce::String(
                                currentKeygroup.env2.*member
                            ),
                            false
                        );

                        return;
                    }

                    if (value ==
                        currentKeygroup.env2.*member)
                    {
                        return;
                    }

                    currentKeygroup.env2.*member =
                        value;

                    DBG(
                        "ENV2 CHANGED = "
                        + juce::String(value)
                    );

                    if (onKeygroupChanged)
                    {
                        onKeygroupChanged(
                            currentKeygroupIndex,
                            currentKeygroup
                        );
                    }
                };
        };

    setupEnv2Editor(
        env2R1Editor,
        &Envelope2::r1
    );

    setupEnv2Editor(
        env2L1Editor,
        &Envelope2::l1
    );

    setupEnv2Editor(
        env2R2Editor,
        &Envelope2::r2
    );

    setupEnv2Editor(
        env2L2Editor,
        &Envelope2::l2
    );

    setupEnv2Editor(
        env2R3Editor,
        &Envelope2::r3
    );

    setupEnv2Editor(
        env2L3Editor,
        &Envelope2::l3
    );

    setupEnv2Editor(
        env2R4Editor,
        &Envelope2::r4
    );

    setupEnv2Editor(
        env2L4Editor,
        &Envelope2::l4
    );


    velocityXFadeLabel.setText(
        "Velocity XFade",
        juce::dontSendNotification
    );

    addAndMakeVisible(velocityXFadeLabel);
    addAndMakeVisible(velocityXFadeEditor);



    env2VelAttackLabel.setText(
        "Vel Attack",
        juce::dontSendNotification
    );

    env2VelReleaseLabel.setText(
        "Vel Release",
        juce::dontSendNotification
    );

    env2NoteOffReleaseLabel.setText(
        "NoteOff Rel",
        juce::dontSendNotification
    );

    env2KeyTrackingLabel.setText(
        "Key Track",
        juce::dontSendNotification
    );

    ePtchLabel.setText(
        "E -> Pitch",
        juce::dontSendNotification
    );

    addAndMakeVisible(ePtchLabel);
    addAndMakeVisible(ePtchEditor);


    addAndMakeVisible(env2VelAttackLabel);
    addAndMakeVisible(env2VelAttackEditor);

    addAndMakeVisible(env2VelReleaseLabel);
    addAndMakeVisible(env2VelReleaseEditor);

    addAndMakeVisible(env2NoteOffReleaseLabel);
    addAndMakeVisible(env2NoteOffReleaseEditor);

    addAndMakeVisible(env2KeyTrackingLabel);
    addAndMakeVisible(env2KeyTrackingEditor);

}


void KeyGroupEditor::setKeygroup(
    const Keygroup& keygroup,
    int keygroupIndex
)
{
    currentKeygroup = keygroup;
    currentKeygroupIndex = keygroupIndex;

    titleLabel.setText(
        "Keygroup "
        + juce::String(keygroupIndex),
        juce::dontSendNotification
    );

    lowNoteEditor.setText(
        juce::String(keygroup.lowNote),
        false
    );

    highNoteEditor.setText(
        juce::String(keygroup.highNote),
        false
    );

    const int cents =
        juce::roundToInt(
            keygroup.tune * 100.0 / 256.0
        );

    tuneEditor.setText(
        juce::String(cents),
        false
    );

    filterFreqEditor.setText(
        juce::String(keygroup.filter.freq),
        false
    );

    filterKeyFollowEditor.setText(
        juce::String(keygroup.filter.keyFollow),
        false
    );

    velocityToFreqEditor.setText(
        juce::String(keygroup.filter.velocityToFreq),
        false
    );

    pressureToFreqEditor.setText(
        juce::String(keygroup.filter.pressureToFreq),
        false
    );

    envelopeToFreqEditor.setText(
        juce::String(keygroup.filter.envelopeToFreq),
        false
    );

    env1AttackEditor.setText(
        juce::String(keygroup.env1.attack),
        false
    );

    env1DecayEditor.setText(
        juce::String(keygroup.env1.decay),
        false
    );

    env1SustainEditor.setText(
        juce::String(keygroup.env1.sustain),
        false
    );

    env1ReleaseEditor.setText(
        juce::String(keygroup.env1.release),
        false
    );

    //env2AttackEditor.setText(
    //    juce::String(keygroup.env2.attack),
    //    false
    //);

    //env2DecayEditor.setText(
    //    juce::String(keygroup.env2.decay),
    //    false
    //);

    //env2SustainEditor.setText(
    //    juce::String(keygroup.env2.sustain),
    //    false
    //);

    //env2ReleaseEditor.setText(
    //    juce::String(keygroup.env2.release),
    //    false
    //);


    env2R1Editor.setText(
        juce::String(keygroup.env2.r1),
        false
    );

    env2L1Editor.setText(
        juce::String(keygroup.env2.l1),
        false
    );

    env2R2Editor.setText(
        juce::String(keygroup.env2.r2),
        false
    );

    env2L2Editor.setText(
        juce::String(keygroup.env2.l2),
        false
    );

    env2R3Editor.setText(
        juce::String(keygroup.env2.r3),
        false
    );

    env2L3Editor.setText(
        juce::String(keygroup.env2.l3),
        false
    );

    env2R4Editor.setText(
        juce::String(keygroup.env2.r4),
        false
    );

    env2L4Editor.setText(
        juce::String(keygroup.env2.l4),
        false
    );

    env2VelAttackEditor.setText(
        juce::String(keygroup.env2.velAttack),
        false
    );

    env2VelReleaseEditor.setText(
        juce::String(keygroup.env2.velRelease),
        false
    );

    env2NoteOffReleaseEditor.setText(
        juce::String(keygroup.env2.noteOffRelease),
        false
    );

    env2KeyTrackingEditor.setText(
        juce::String(keygroup.env2.keyTracking),
        false
    );

    velocityEnv2Editor.setText(
        juce::String(keygroup.velocity.vEnv2),
        false
    );

    ePtchEditor.setText(
        juce::String(keygroup.velocity.ePtch),
        false
    );

    velocityXFadeEditor.setText(
        juce::String(keygroup.velocity.vxFade),
        false
    );

    velocityZonesEditor.setText(
        juce::String(keygroup.velocity.vZones),
        false
    );

    leftKeyXFadeEditor.setText(
        juce::String(keygroup.velocity.lkxf),
        false
    );

    rightKeyXFadeEditor.setText(
        juce::String(keygroup.velocity.rkxf),
        false
    );





}


void KeyGroupEditor::resized()
{
    const int fieldLabelWidth = 110;
    const int editorWidth = 130;
    const int editorHeight = 24;
    const int envLabelWidth = 65;
    const int envEditorWidth = 100;

    auto area =
        getLocalBounds().reduced(10);

    titleLabel.setBounds(
        area.removeFromTop(30)
    );

    const int rowHeight = 26;
    const int labelWidth = 120;

    auto addRow =
        [&area, rowHeight, labelWidth]
        (
            juce::Label& label,
            auto& editor
            )
        {
            auto row =
                area.removeFromTop(rowHeight);

            label.setBounds(
                row.removeFromLeft(labelWidth)
            );

            editor.setBounds(row);
        };
    keyRangeSectionLabel.setBounds(
        area.removeFromTop(24)
    );


    {
        auto section =
            area.removeFromTop(36);

        auto row =
            section.removeFromTop(30);

        const int itemWidth =
            row.getWidth() / 3;

        auto lowArea =
            row.removeFromLeft(itemWidth);

        auto highArea =
            row.removeFromLeft(itemWidth);

        auto tuneArea =
            row;


        // Low Note
        lowNoteLabel.setBounds(
            lowArea.removeFromLeft(fieldLabelWidth)
        );

        lowNoteEditor.setBounds(
            lowArea.removeFromLeft(editorWidth)
            
        );


        // High Note
        highNoteLabel.setBounds(
            highArea.removeFromLeft(fieldLabelWidth)
        );

        highNoteEditor.setBounds(
            highArea.removeFromLeft(editorWidth)
            
        );


        // Tune
        tuneLabel.setBounds(
            tuneArea.removeFromLeft(fieldLabelWidth)
        );

        tuneEditor.setBounds(
            tuneArea.removeFromLeft(editorWidth)
            
        );
    }

    filterSectionLabel.setBounds(
        area.removeFromTop(24)
    );


{
    auto section =
        area.removeFromTop(64);

    // 1行目
    auto row1 =
        section.removeFromTop(30);

    const int itemWidth =
        row1.getWidth() / 3;

    auto freqArea =
        row1.removeFromLeft(itemWidth);

    auto keyFollowArea =
        row1.removeFromLeft(itemWidth);

    auto velocityArea =
        row1;


    filterFreqLabel.setBounds(
        freqArea.removeFromLeft(fieldLabelWidth)
    );

    filterFreqEditor.setBounds(
        freqArea.removeFromLeft(editorWidth)
        
    );


    filterKeyFollowLabel.setBounds(
        keyFollowArea.removeFromLeft(fieldLabelWidth)
    );

    filterKeyFollowEditor.setBounds(
        keyFollowArea.removeFromLeft(editorWidth)
    );


    velocityToFreqLabel.setBounds(
        velocityArea.removeFromLeft(fieldLabelWidth)
    );

    velocityToFreqEditor.setBounds(
        velocityArea.removeFromLeft(editorWidth)
    );


    // 2行目
    auto row2 =
        section.removeFromTop(30);

    auto pressureArea =
        row2.removeFromLeft(itemWidth);

    auto envelopeArea =
        row2.removeFromLeft(itemWidth);


    pressureToFreqLabel.setBounds(
        pressureArea.removeFromLeft(fieldLabelWidth)
    );

    pressureToFreqEditor.setBounds(
        pressureArea.removeFromLeft(editorWidth)
    );


    envelopeToFreqLabel.setBounds(
        envelopeArea.removeFromLeft(fieldLabelWidth)
    );

    envelopeToFreqEditor.setBounds(
        envelopeArea.removeFromLeft(editorWidth)
    );
}

env1SectionLabel.setBounds(
    area.removeFromTop(24)
);


// =========================
// ENV1
// =========================
{
    auto section =
        area.removeFromTop(36);

    auto row =
        section.removeFromTop(30);

    const int itemWidth =
        row.getWidth() / 4;

    auto attackArea =
        row.removeFromLeft(itemWidth);

    auto decayArea =
        row.removeFromLeft(itemWidth);

    auto sustainArea =
        row.removeFromLeft(itemWidth);

    auto releaseArea =
        row;

    // Attack
    env1AttackLabel.setBounds(
        attackArea.removeFromLeft(envLabelWidth)
    );

    env1AttackEditor.setBounds(
        attackArea.removeFromLeft(envEditorWidth)
    );

    // Decay
    env1DecayLabel.setBounds(
        decayArea.removeFromLeft(envLabelWidth)
    );

    env1DecayEditor.setBounds(
        decayArea.removeFromLeft(envEditorWidth)
    );

    // Sustain
    env1SustainLabel.setBounds(
        sustainArea.removeFromLeft(envLabelWidth)
    );

    env1SustainEditor.setBounds(
        sustainArea.removeFromLeft(envEditorWidth)
    );

    // Release
    env1ReleaseLabel.setBounds(
        releaseArea.removeFromLeft(envLabelWidth)
    );

    env1ReleaseEditor.setBounds(
        releaseArea.removeFromLeft(envEditorWidth)
    );
}

area.removeFromTop(6);

    /*ddRow(env2AttackLabel, env2AttackEditor);
    addRow(env2DecayLabel, env2DecayEditor);
    addRow(env2SustainLabel, env2SustainEditor);
    addRow(env2ReleaseLabel, env2ReleaseEditor);*/

    /*addRow(env2R1Label, env2R1Editor);
    addRow(env2L1Label, env2L1Editor);

    addRow(env2R2Label, env2R2Editor);
    addRow(env2L2Label, env2L2Editor);

    addRow(env2R3Label, env2R3Editor);
    addRow(env2L3Label, env2L3Editor);

    addRow(env2R4Label, env2R4Editor);
    addRow(env2L4Label, env2L4Editor);

    addRow(env2VelAttackLabel, env2VelAttackEditor);
    addRow(env2VelReleaseLabel, env2VelReleaseEditor);
    addRow(env2NoteOffReleaseLabel, env2NoteOffReleaseEditor);
    addRow(env2KeyTrackingLabel, env2KeyTrackingEditor);*/

env2SectionLabel.setBounds(
    area.removeFromTop(24)
);

    // =========================
// ENV2
// =========================
{
    auto section =
        area.removeFromTop(96);

    const int envLabelWidth = 70;
    const int envEditorWidth = 90;

    // -------------------------
    // Row 1
    // R1 / L1 / R2 / L2
    // -------------------------

    auto row1 =
        section.removeFromTop(30);

    const int itemWidth =
        row1.getWidth() / 4;

    auto r1Area =
        row1.removeFromLeft(itemWidth);

    auto l1Area =
        row1.removeFromLeft(itemWidth);

    auto r2Area =
        row1.removeFromLeft(itemWidth);

    auto l2Area =
        row1;

    env2R1Label.setBounds(
        r1Area.removeFromLeft(envLabelWidth)
    );

    env2R1Editor.setBounds(
        r1Area.removeFromLeft(envEditorWidth)
    );

    env2L1Label.setBounds(
        l1Area.removeFromLeft(envLabelWidth)
    );

    env2L1Editor.setBounds(
        l1Area.removeFromLeft(envEditorWidth)
    );

    env2R2Label.setBounds(
        r2Area.removeFromLeft(envLabelWidth)
    );

    env2R2Editor.setBounds(
        r2Area.removeFromLeft(envEditorWidth)
    );

    env2L2Label.setBounds(
        l2Area.removeFromLeft(envLabelWidth)
    );

    env2L2Editor.setBounds(
        l2Area.removeFromLeft(envEditorWidth)
    );


    // -------------------------
    // Row 2
    // R3 / L3 / R4 / L4
    // -------------------------

    auto row2 =
        section.removeFromTop(30);

    auto r3Area =
        row2.removeFromLeft(itemWidth);

    auto l3Area =
        row2.removeFromLeft(itemWidth);

    auto r4Area =
        row2.removeFromLeft(itemWidth);

    auto l4Area =
        row2;

    env2R3Label.setBounds(
        r3Area.removeFromLeft(envLabelWidth)
    );

    env2R3Editor.setBounds(
        r3Area.removeFromLeft(envEditorWidth)
    );

    env2L3Label.setBounds(
        l3Area.removeFromLeft(envLabelWidth)
    );

    env2L3Editor.setBounds(
        l3Area.removeFromLeft(envEditorWidth)
    );

    env2R4Label.setBounds(
        r4Area.removeFromLeft(envLabelWidth)
    );

    env2R4Editor.setBounds(
        r4Area.removeFromLeft(envEditorWidth)
    );

    env2L4Label.setBounds(
        l4Area.removeFromLeft(envLabelWidth)
    );

    env2L4Editor.setBounds(
        l4Area.removeFromLeft(envEditorWidth)
    );


    // -------------------------
    // Row 3
    // Vel Attack / Vel Release
    // NoteOff Release / Key Track
    // -------------------------

    auto row3 =
        section.removeFromTop(30);

    auto velAttackArea =
        row3.removeFromLeft(itemWidth);

    auto velReleaseArea =
        row3.removeFromLeft(itemWidth);

    auto noteOffArea =
        row3.removeFromLeft(itemWidth);

    auto keyTrackArea =
        row3;

    env2VelAttackLabel.setBounds(
        velAttackArea.removeFromLeft(envLabelWidth)
    );

    env2VelAttackEditor.setBounds(
        velAttackArea.removeFromLeft(envEditorWidth)
    );

    env2VelReleaseLabel.setBounds(
        velReleaseArea.removeFromLeft(envLabelWidth)
    );

    env2VelReleaseEditor.setBounds(
        velReleaseArea.removeFromLeft(envEditorWidth)
    );

    env2NoteOffReleaseLabel.setBounds(
        noteOffArea.removeFromLeft(envLabelWidth)
    );

    env2NoteOffReleaseEditor.setBounds(
        noteOffArea.removeFromLeft(envEditorWidth)
    );

    env2KeyTrackingLabel.setBounds(
        keyTrackArea.removeFromLeft(envLabelWidth)
    );

    env2KeyTrackingEditor.setBounds(
        keyTrackArea.removeFromLeft(envEditorWidth)
    );
}

area.removeFromTop(6);

modulationSectionLabel.setBounds(
    area.removeFromTop(24)
);

// =========================
// MODULATION
// =========================
{
    auto row =
        area.removeFromTop(30);

    const int itemWidth =
        row.getWidth() / 4;

    const int modLabelWidth = 100;
    const int modEditorWidth = 90;

    auto velEnvArea =
        row.removeFromLeft(itemWidth);

    auto pitchArea =
        row.removeFromLeft(itemWidth);

    auto velXFadeArea =
        row.removeFromLeft(itemWidth);

    auto velZonesArea =
        row;


    velocityEnv2Label.setBounds(
        velEnvArea.removeFromLeft(modLabelWidth)
    );

    velocityEnv2Editor.setBounds(
        velEnvArea.removeFromLeft(modEditorWidth)
    );


    ePtchLabel.setBounds(
        pitchArea.removeFromLeft(modLabelWidth)
    );

    ePtchEditor.setBounds(
        pitchArea.removeFromLeft(modEditorWidth)
    );


    velocityXFadeLabel.setBounds(
        velXFadeArea.removeFromLeft(modLabelWidth)
    );

    velocityXFadeEditor.setBounds(
        velXFadeArea.removeFromLeft(modEditorWidth)
    );


    velocityZonesLabel.setBounds(
        velZonesArea.removeFromLeft(modLabelWidth)
    );

    velocityZonesEditor.setBounds(
        velZonesArea.removeFromLeft(modEditorWidth)
    );
}

area.removeFromTop(6);

area.removeFromTop(6);

keyXFadeSectionLabel.setBounds(
    area.removeFromTop(24)
);


// =========================
// KEY XFADE
// =========================
{
    auto row =
        area.removeFromTop(30);

    const int itemWidth =
        row.getWidth() / 2;

    const int xfadeLabelWidth = 100;
    const int xfadeEditorWidth = 90;

    auto leftArea =
        row.removeFromLeft(itemWidth);

    auto rightArea =
        row;


    leftKeyXFadeLabel.setBounds(
        leftArea.removeFromLeft(xfadeLabelWidth)
    );

    leftKeyXFadeEditor.setBounds(
        leftArea.removeFromLeft(xfadeEditorWidth)
    );


    rightKeyXFadeLabel.setBounds(
        rightArea.removeFromLeft(xfadeLabelWidth)
    );

    rightKeyXFadeEditor.setBounds(
        rightArea.removeFromLeft(xfadeEditorWidth)
    );
}




}


void KeyGroupEditor::paint(
    juce::Graphics& g)
{
    g.setColour(
        juce::Colours::white.withAlpha(0.18f)
    );

    auto drawSectionLine =
        [&g](const juce::Label& label)
        {
            const float y =
                static_cast<float>(
                    label.getY()
                    + label.getHeight() / 2
                    );

            const float startX =
                static_cast<float>(
                    label.getX() + 110
                    );

            const float endX =
                static_cast<float>(
                    label.getRight()
                    );

            g.drawLine(
                startX,
                y,
                endX,
                y,
                1.0f
            );
        };

    drawSectionLine(keyRangeSectionLabel);
    drawSectionLine(filterSectionLabel);
    drawSectionLine(env1SectionLabel);
    drawSectionLine(env2SectionLabel);
    drawSectionLine(modulationSectionLabel);
    drawSectionLine(keyXFadeSectionLabel);
}