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

            if (value < 21 || value > 127)
            {
                lowNoteEditor.setText(
                    juce::String(currentKeygroup.lowNote),
                    false
                );
                return;
            }

            int newLow = value;
            int newHigh = currentKeygroup.highNote;

            if (newLow > newHigh)
                newHigh = newLow;

            if (newLow == currentKeygroup.lowNote &&
                newHigh == currentKeygroup.highNote)
            {
                return;
            }

            currentKeygroup.lowNote = newLow;
            currentKeygroup.highNote = newHigh;

            lowNoteEditor.setText(
                juce::String(newLow),
                juce::dontSendNotification
            );

            highNoteEditor.setText(
                juce::String(newHigh),
                juce::dontSendNotification
            );

            if (onKeyRangeChanged)
                onKeyRangeChanged(newLow, newHigh);
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

            int newLow = currentKeygroup.lowNote;
            int newHigh = value;

            // HIGHÇLOWÇÊÇËâ∫Ç∞ÇΩèÍçáÅALOWÇ‡í«è]
            if (newHigh < newLow)
                newLow = newHigh;

            if (newLow == currentKeygroup.lowNote &&
                newHigh == currentKeygroup.highNote)
            {
                return;
            }

            currentKeygroup.lowNote = newLow;
            currentKeygroup.highNote = newHigh;

            lowNoteEditor.setText(
                juce::String(newLow),
                juce::dontSendNotification
            );

            highNoteEditor.setText(
                juce::String(newHigh),
                juce::dontSendNotification
            );

            if (onKeyRangeChanged)
                onKeyRangeChanged(newLow, newHigh);
        };

    addAndMakeVisible(filterFreqLabel);
    addAndMakeVisible(filterFreqKnob);

    addAndMakeVisible(filterKeyFollowLabel);
    addAndMakeVisible(filterKeyFollowKnob);

    resonanceLabel.setText(
        "Resonance",
        juce::dontSendNotification
    );

    resonanceLabel.setJustificationType(
        juce::Justification::centred
    );

    addAndMakeVisible(resonanceLabel);
    addAndMakeVisible(resonanceKnob);

    addAndMakeVisible(filterEditModeCombo);

    filterEditModeCombo.addItem("ONE", 1);
    filterEditModeCombo.addItem("ALL", 2);

    filterEditModeCombo.setSelectedId(
        1,
        juce::dontSendNotification
    );


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

    addAndMakeVisible(env2Editor);

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

    //velocityToFreqLabel.setText(
    //    "Velocity -> Freq",
    //    juce::dontSendNotification
    //);

    //pressureToFreqLabel.setText(
    //    "Pressure -> Freq",
    //    juce::dontSendNotification
    //);

    //envelopeToFreqLabel.setText(
    //    "Envelope -> Freq",
    //    juce::dontSendNotification
    //);

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

    env2Editor.onEnvelopeChanged =
        [this](
            int r1, int l1,
            int r2, int l2,
            int r3, int l3,
            int r4, int l4)
        {
            const bool r1Changed =
                currentKeygroup.env2.r1 != r1;

            const bool l1Changed =
                currentKeygroup.env2.l1 != l1;

            const bool r2Changed =
                currentKeygroup.env2.r2 != r2;

            const bool l2Changed =
                currentKeygroup.env2.l2 != l2;

            const bool r3Changed =
                currentKeygroup.env2.r3 != r3;

            const bool l3Changed =
                currentKeygroup.env2.l3 != l3;

            const bool r4Changed =
                currentKeygroup.env2.r4 != r4;

            const bool l4Changed =
                currentKeygroup.env2.l4 != l4;


            currentKeygroup.env2.r1 = r1;
            currentKeygroup.env2.l1 = l1;

            if (r1Changed && onEnv2R1Changed)
            {
                onEnv2R1Changed(
                    currentKeygroupIndex,
                    r1
                );
            }

            if (l1Changed && onEnv2L1Changed)
            {
                onEnv2L1Changed(
                    currentKeygroupIndex,
                    l1
                );
            }

            if (r2Changed && onEnv2R2Changed)
            {
                onEnv2R2Changed(
                    currentKeygroupIndex,
                    r2
                );
            }

            if (l2Changed && onEnv2L2Changed)
            {
                onEnv2L2Changed(
                    currentKeygroupIndex,
                    l2
                );
            }

            if (r3Changed && onEnv2R3Changed)
            {
                onEnv2R3Changed(
                    currentKeygroupIndex,
                    r3
                );
            }

            if (l3Changed && onEnv2L3Changed)
            {
                onEnv2L3Changed(
                    currentKeygroupIndex,
                    l3
                );
            }

            if (r4Changed && onEnv2R4Changed)
            {
                onEnv2R4Changed(
                    currentKeygroupIndex,
                    r4
                );
            }

            if (l4Changed && onEnv2L4Changed)
            {
                onEnv2L4Changed(
                    currentKeygroupIndex,
                    l4
                );
            }


            currentKeygroup.env2.r2 = r2;
            currentKeygroup.env2.l2 = l2;

            currentKeygroup.env2.r3 = r3;
            currentKeygroup.env2.l3 = l3;

            currentKeygroup.env2.r4 = r4;
            currentKeygroup.env2.l4 = l4;

            env2R1Editor.setText(
                juce::String(r1), false);

            env2L1Editor.setText(
                juce::String(l1), false);

            env2R2Editor.setText(
                juce::String(r2), false);

            env2L2Editor.setText(
                juce::String(l2), false);

            env2R3Editor.setText(
                juce::String(r3), false);

            env2L3Editor.setText(
                juce::String(l3), false);

            env2R4Editor.setText(
                juce::String(r4), false);

            env2L4Editor.setText(
                juce::String(l4), false);
        };

    env2Editor.onEditFinished =
        [this]()
        {
            DBG("ENV2 EDIT FINISHED");
        };


    //addAndMakeVisible(velocityToFreqLabel);
    //addAndMakeVisible(velocityToFreqKnob);

    //addAndMakeVisible(pressureToFreqLabel);
    //addAndMakeVisible(pressureToFreqKnob);

    //addAndMakeVisible(envelopeToFreqLabel);
    //addAndMakeVisible(envelopeToFreqKnob);

    auto setupFilterKnob =
        [this](
            juce::Slider& knob,
            double minimum,
            double maximum
            )
        {
            knob.setSliderStyle(
                juce::Slider::RotaryHorizontalVerticalDrag
            );

            knob.setTextBoxStyle(
                juce::Slider::TextBoxBelow,
                false,
                58,
                20
            );

            knob.setRange(
                minimum,
                maximum,
                1.0
            );

            knob.setNumDecimalPlacesToDisplay(0);

            knob.setDoubleClickReturnValue(
                true,
                0.0
            );

            knob.setColour(
                juce::Slider::rotarySliderFillColourId,
                juce::Colour(0xff51a9c9)
            );

            knob.setColour(
                juce::Slider::rotarySliderOutlineColourId,
                juce::Colour(0xff26343a)
            );

            knob.setColour(
                juce::Slider::thumbColourId,
                juce::Colours::white
            );
        };



    setupFilterKnob(filterFreqKnob, 0.0, 99.0);
    setupFilterKnob(filterKeyFollowKnob, 0.0, 12.0);
    //setupFilterKnob(velocityToFreqKnob, -50.0, 50.0);
 /*   setupFilterKnob(pressureToFreqKnob, -50.0, 50.0);
    setupFilterKnob(envelopeToFreqKnob, -50.0, 50.0);*/
    setupFilterKnob(resonanceKnob, 0.0, 15.0);

    filterFreqLabel.setJustificationType(
        juce::Justification::centred
    );

    filterKeyFollowLabel.setJustificationType(
        juce::Justification::centred
    );

    //velocityToFreqLabel.setJustificationType(
    //    juce::Justification::centred
    //);

    //pressureToFreqLabel.setJustificationType(
    //    juce::Justification::centred
    //);

    //envelopeToFreqLabel.setJustificationType(
    //    juce::Justification::centred
    //);

    resonanceKnob.onValueChange = [this]()
        {
            const int value =
                juce::roundToInt(
                    resonanceKnob.getValue()
                );

            currentKeygroup.filter.resonance = value;

            pendingResonance = value;

            DBG(
                "FILTER RESONANCE QUEUED = "
                + juce::String(value)
            );

            if (!isFilterEditAll() &&
                !isTimerRunning())
            {
                startTimer(15);
            }
        };

    resonanceKnob.onDragEnd = [this]()
        {
            const int value =
                juce::roundToInt(
                    resonanceKnob.getValue()
                );

            pendingResonance = -1;

            DBG(
                "FILTER RESONANCE FINAL SEND = "
                + juce::String(value)
            );

            if (value != lastSentResonance)
            {
                lastSentResonance = value;

                if (onResonanceChanged)
                {
                    onResonanceChanged(
                        currentKeygroupIndex,
                        value
                    );
                }
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

    filterFreqKnob.onValueChange = [this]()
        {
            const int value =
                juce::roundToInt(filterFreqKnob.getValue());

            currentKeygroup.filter.freq = value;

            pendingFilterFreq = value;

            DBG(
                "FILTER FREQ QUEUED = "
                + juce::String(value)
            );

            if (!isFilterEditAll() &&
                !isTimerRunning())
            {
                startTimer(15);
            }
        };

    filterFreqKnob.onDragEnd = [this]()
        {
            stopTimer();

            const int value =
                juce::roundToInt(filterFreqKnob.getValue());

            pendingFilterFreq = -1;

            DBG(
                "FILTER FREQ FINAL SEND = "
                + juce::String(value)
            );

            if (value != lastSentFilterFreq)
            {
                lastSentFilterFreq = value;

                if (onFilterFreqChanged)
                {
                    onFilterFreqChanged(
                        currentKeygroupIndex,
                        value
                    );
                }
            }
        };

    auto sendKeygroupChange = [this]()
        {
            if (onKeygroupChanged)
            {
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };

    filterKeyFollowKnob.onValueChange =
        [this]()
        {
            currentKeygroup.filter.keyFollow =
                juce::roundToInt(
                    filterKeyFollowKnob.getValue()
                );
        };

    filterKeyFollowKnob.onDragEnd =
        [this]()
        {
            if (onFilterKeyFollowChanged)
            {
                onFilterKeyFollowChanged(
                    currentKeygroupIndex,
                    currentKeygroup.filter.keyFollow
                );
            }
        };

    /*filterKeyFollowKnob.onValueChange = [this]()
        {
            const int value =
                juce::roundToInt(filterKeyFollowKnob.getValue());

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
        };*/

    //velocityToFreqKnob.onValueChange =
    //    [this, sendKeygroupChange]()
    //    {
    //        currentKeygroup.filter.velocityToFreq =
    //            juce::roundToInt(
    //                velocityToFreqKnob.getValue()
    //            );

    //        if (!velocityToFreqKnob.isMouseButtonDown())
    //            sendKeygroupChange();
    //    };

    //velocityToFreqKnob.onDragEnd =
    //    [sendKeygroupChange]()
    //    {
    //        sendKeygroupChange();
    //    };

    //pressureToFreqKnob.onValueChange =
    //    [this, sendKeygroupChange]()
    //    {
    //        currentKeygroup.filter.pressureToFreq =
    //            juce::roundToInt(
    //                pressureToFreqKnob.getValue()
    //            );

    //        if (!pressureToFreqKnob.isMouseButtonDown())
    //            sendKeygroupChange();
    //    };

    //pressureToFreqKnob.onDragEnd =
    //    [sendKeygroupChange]()
    //    {
    //        sendKeygroupChange();
    //    };

    //envelopeToFreqKnob.onValueChange =
    //    [this, sendKeygroupChange]()
    //    {
    //        currentKeygroup.filter.envelopeToFreq =
    //            juce::roundToInt(
    //                envelopeToFreqKnob.getValue()
    //            );

    //        if (!envelopeToFreqKnob.isMouseButtonDown())
    //            sendKeygroupChange();
    //    };

    //envelopeToFreqKnob.onDragEnd =
    //    [sendKeygroupChange]()
    //    {
    //        sendKeygroupChange();
    //    };
   

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
            DBG("E_PTCH FOCUS LOST");
            const int value =
                ePtchEditor.getText().getIntValue();

            if (value < -50 || value > 50)
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

    ePtchEditor.onTextChange =
        [this]()
        {
            DBG(
                "E_PTCH TEXT CHANGED = "
                + ePtchEditor.getText()
            );
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

                    if (member == &Envelope2::r1)
                    {
                        if (onEnv2R1Changed)
                        {
                            onEnv2R1Changed(
                                currentKeygroupIndex,
                                value
                            );
                        }
                    }
                    else if (member == &Envelope2::l1)
                    {
                        if (onEnv2L1Changed)
                        {
                            onEnv2L1Changed(
                                currentKeygroupIndex,
                                value
                            );
                        }
                    }
                    else if (member == &Envelope2::r2)
                    {
                        if (onEnv2R2Changed)
                        {
                            onEnv2R2Changed(
                                currentKeygroupIndex,
                                value
                            );
                        }
                    }

                    else if (member == &Envelope2::l2)
                    {
                        if (onEnv2L2Changed)
                            onEnv2L2Changed(
                                currentKeygroupIndex,
                                value
                            );

                        return;
                    }

                    else if (member == &Envelope2::r3)
                    {
                        if (onEnv2R3Changed)
                            onEnv2R3Changed(currentKeygroupIndex, value);
                        return;
                    }
                    else if (member == &Envelope2::l3)
                    {
                        if (onEnv2L3Changed)
                            onEnv2L3Changed(currentKeygroupIndex, value);
                        return;
                    }
                    else if (member == &Envelope2::r4)
                    {
                        if (onEnv2R4Changed)
                            onEnv2R4Changed(currentKeygroupIndex, value);
                        return;
                    }
                    else if (member == &Envelope2::l4)
                    {
                        if (onEnv2L4Changed)
                            onEnv2L4Changed(currentKeygroupIndex, value);
                        return;
                    }


                    else
                    {
                        if (onKeygroupChanged)
                        {
                            onKeygroupChanged(
                                currentKeygroupIndex,
                                currentKeygroup
                            );
                        }
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

    ePtchEditor.toFront(false);


    addAndMakeVisible(env2VelAttackLabel);
    addAndMakeVisible(env2VelAttackEditor);

    addAndMakeVisible(env2VelReleaseLabel);
    addAndMakeVisible(env2VelReleaseEditor);

    addAndMakeVisible(env2NoteOffReleaseLabel);
    addAndMakeVisible(env2NoteOffReleaseEditor);

    addAndMakeVisible(env2KeyTrackingLabel);
    addAndMakeVisible(env2KeyTrackingEditor);

    addAndMakeVisible(env1Editor);

    env1Editor.onEnvelopeChanged =
        [this](
            int attack,
            int decay,
            int sustain,
            int release)
        {
            currentKeygroup.env1.attack = attack;
            currentKeygroup.env1.decay = decay;
            currentKeygroup.env1.sustain = sustain;
            currentKeygroup.env1.release = release;

            env1AttackEditor.setText(
                juce::String(attack), false);

            env1DecayEditor.setText(
                juce::String(decay), false);

            env1SustainEditor.setText(
                juce::String(sustain), false);

            env1ReleaseEditor.setText(
                juce::String(release), false);
        };

    env1Editor.onEditFinished =
        [this]()
        {
            if (onKeygroupChanged)
            {
                onKeygroupChanged(
                    currentKeygroupIndex,
                    currentKeygroup
                );
            }
        };

}


void KeyGroupEditor::timerCallback()
{
    // Filter Frequency
    if (pendingFilterFreq >= 0)
    {
        const int value = pendingFilterFreq;

        if (value != lastSentFilterFreq)
        {
            lastSentFilterFreq = value;

            DBG(
                "FILTER FREQ LIVE SEND = "
                + juce::String(value)
            );

            if (onFilterFreqChanged)
            {
                onFilterFreqChanged(
                    currentKeygroupIndex,
                    value
                );
            }
        }
    }

    if (pendingResonance >= 0)
    {
        const int value =
            pendingResonance;

        if (value != lastSentResonance)
        {
            lastSentResonance = value;

            DBG(
                "FILTER RESONANCE LIVE SEND = "
                + juce::String(value)
            );

            if (onResonanceChanged)
            {
                onResonanceChanged(
                    currentKeygroupIndex,
                    value
                );
            }
        }
    }

    // Filter Key Follow
    if (pendingFilterKeyFollow >= 0)
    {
        const int value = pendingFilterKeyFollow;

        if (value != lastSentFilterKeyFollow)
        {
            lastSentFilterKeyFollow = value;

            DBG(
                "FILTER KEY FOLLOW LIVE SEND = "
                + juce::String(value)
            );

            if (onFilterKeyFollowChanged)
            {
                onFilterKeyFollowChanged(
                    currentKeygroupIndex,
                    value
                );
            }
        }
    }
}


void KeyGroupEditor::setEnv2R2(int value)
{
    currentKeygroup.env2.r2 = value;

    env2R2Editor.setText(
        juce::String(value),
        false
    );

    env2Editor.setEnvelope(
        currentKeygroup.env2.r1,
        currentKeygroup.env2.l1,
        currentKeygroup.env2.r2,
        currentKeygroup.env2.l2,
        currentKeygroup.env2.r3,
        currentKeygroup.env2.l3,
        currentKeygroup.env2.r4,
        currentKeygroup.env2.l4
    );

    DBG("UI ENV2 R2 = " + juce::String(value));
}

void KeyGroupEditor::setEnv2L1(int value)
{
    currentKeygroup.env2.l1 = value;

    env2L1Editor.setText(
        juce::String(value),
        false
    );

    env2Editor.setEnvelope(
        currentKeygroup.env2.r1,
        currentKeygroup.env2.l1,
        currentKeygroup.env2.r2,
        currentKeygroup.env2.l2,
        currentKeygroup.env2.r3,
        currentKeygroup.env2.l3,
        currentKeygroup.env2.r4,
        currentKeygroup.env2.l4
    );

    DBG("UI ENV2 L1 = " + juce::String(value));
}


void KeyGroupEditor::setEnv2R1(int value)
{
    currentKeygroup.env2.r1 = value;

    env2R1Editor.setText(
        juce::String(value),
        false
    );

    env2Editor.setEnvelope(
        currentKeygroup.env2.r1,
        currentKeygroup.env2.l1,
        currentKeygroup.env2.r2,
        currentKeygroup.env2.l2,
        currentKeygroup.env2.r3,
        currentKeygroup.env2.l3,
        currentKeygroup.env2.r4,
        currentKeygroup.env2.l4
    );

    DBG("UI ENV2 R1 = " + juce::String(value));
}

void KeyGroupEditor::setEnv2L2(int value)
{
    currentKeygroup.env2.l2 = value;

    env2L2Editor.setText(
        juce::String(value),
        false
    );

    env2Editor.setEnvelope(
        currentKeygroup.env2.r1,
        currentKeygroup.env2.l1,
        currentKeygroup.env2.r2,
        currentKeygroup.env2.l2,
        currentKeygroup.env2.r3,
        currentKeygroup.env2.l3,
        currentKeygroup.env2.r4,
        currentKeygroup.env2.l4
    );

    DBG("UI ENV2 L2 = " + juce::String(value));
}

void KeyGroupEditor::setEnv2R3(int value)
{
    currentKeygroup.env2.r3 = value;

    env2R3Editor.setText(
        juce::String(value),
        false
    );

    env2Editor.setEnvelope(
        currentKeygroup.env2.r1,
        currentKeygroup.env2.l1,
        currentKeygroup.env2.r2,
        currentKeygroup.env2.l2,
        currentKeygroup.env2.r3,
        currentKeygroup.env2.l3,
        currentKeygroup.env2.r4,
        currentKeygroup.env2.l4
    );

    DBG("UI ENV2 R3 = " + juce::String(value));
}

void KeyGroupEditor::setEnv2L3(int value)
{
    currentKeygroup.env2.l3 = value;

    env2L3Editor.setText(
        juce::String(value),
        false
    );

    env2Editor.setEnvelope(
        currentKeygroup.env2.r1,
        currentKeygroup.env2.l1,
        currentKeygroup.env2.r2,
        currentKeygroup.env2.l2,
        currentKeygroup.env2.r3,
        currentKeygroup.env2.l3,
        currentKeygroup.env2.r4,
        currentKeygroup.env2.l4
    );

    DBG("UI ENV2 L3 = " + juce::String(value));
}

void KeyGroupEditor::setEnv2R4(int value)
{
    currentKeygroup.env2.r4 = value;

    env2R4Editor.setText(
        juce::String(value),
        false
    );

    env2Editor.setEnvelope(
        currentKeygroup.env2.r1,
        currentKeygroup.env2.l1,
        currentKeygroup.env2.r2,
        currentKeygroup.env2.l2,
        currentKeygroup.env2.r3,
        currentKeygroup.env2.l3,
        currentKeygroup.env2.r4,
        currentKeygroup.env2.l4
    );

    DBG("UI ENV2 R4 = " + juce::String(value));
}

void KeyGroupEditor::setEnv2L4(int value)
{
    currentKeygroup.env2.l4 = value;

    env2L4Editor.setText(
        juce::String(value),
        false
    );

    env2Editor.setEnvelope(
        currentKeygroup.env2.r1,
        currentKeygroup.env2.l1,
        currentKeygroup.env2.r2,
        currentKeygroup.env2.l2,
        currentKeygroup.env2.r3,
        currentKeygroup.env2.l3,
        currentKeygroup.env2.r4,
        currentKeygroup.env2.l4
    );

    DBG("UI ENV2 L4 = " + juce::String(value));
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
        + juce::String(keygroupIndex + 1),
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

    filterFreqKnob.setValue(
        keygroup.filter.freq,
        juce::dontSendNotification
    );

    filterKeyFollowKnob.setValue(
        keygroup.filter.keyFollow,
        juce::dontSendNotification
    );

    resonanceKnob.setValue(
        keygroup.filter.resonance,
        juce::dontSendNotification
    );

    //velocityToFreqKnob.setValue(
    //    keygroup.filter.velocityToFreq,
    //    juce::dontSendNotification
    //);

    //pressureToFreqKnob.setValue(
    //    keygroup.filter.pressureToFreq,
    //    juce::dontSendNotification
    //);

    //envelopeToFreqKnob.setValue(
    //    keygroup.filter.envelopeToFreq,
    //    juce::dontSendNotification
    //);

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

    env1Editor.setEnvelope(
        keygroup.env1.attack,
        keygroup.env1.decay,
        keygroup.env1.sustain,
        keygroup.env1.release
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

    env2Editor.setEnvelope(
        keygroup.env2.r1,
        keygroup.env2.l1,
        keygroup.env2.r2,
        keygroup.env2.l2,
        keygroup.env2.r3,
        keygroup.env2.l3,
        keygroup.env2.r4,
        keygroup.env2.l4
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

    filterEditModeCombo.setBounds(
        20,
        20,
        80,
        24
    );

    filterSectionLabel.setBounds(
        area.removeFromTop(24)
    );


    {
        auto section =
            area.removeFromTop(130);

        constexpr int numberOfKnobs = 6;

        const int itemWidth =
            section.getWidth() / numberOfKnobs;

        auto layoutKnob =
            [itemWidth](
                juce::Rectangle<int> itemArea,
                juce::Label& label,
                juce::Slider& knob
                )
            {
                itemArea.reduce(5, 0);

                label.setBounds(
                    itemArea.removeFromTop(22)
                );

                knob.setBounds(itemArea);
            };

        auto freqArea =
            section.removeFromLeft(itemWidth);

        auto keyFollowArea =
            section.removeFromLeft(itemWidth);

        auto velocityArea =
            section.removeFromLeft(itemWidth);

        auto pressureArea =
            section.removeFromLeft(itemWidth);

        auto envelopeArea =
            section.removeFromLeft(itemWidth);

        auto resonanceArea =
            section;

        layoutKnob(
            freqArea,
            filterFreqLabel,
            filterFreqKnob
        );



        layoutKnob(
            keyFollowArea,
            filterKeyFollowLabel,
            filterKeyFollowKnob
        );

        //layoutKnob(
        //    velocityArea,
        //    velocityToFreqLabel,
        //    velocityToFreqKnob
        //);

        //layoutKnob(
        //    pressureArea,
        //    pressureToFreqLabel,
        //    pressureToFreqKnob
        //);

        //layoutKnob(
        //    envelopeArea,
        //    envelopeToFreqLabel,
        //    envelopeToFreqKnob
        //);

        layoutKnob(
            resonanceArea,
            resonanceLabel,
            resonanceKnob
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
        area.removeFromTop(220);

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

    section.removeFromTop(10);

    env1Editor.setBounds(
        section.removeFromTop(180)
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
        area.removeFromTop(280);

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

    section.removeFromTop(10);

    env2Editor.setBounds(
        section.removeFromTop(180)
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

void KeyGroupEditor::setResonance(int value)
{
    resonanceKnob.setValue(
        value,
        juce::dontSendNotification
    );
}

void KeyGroupEditor::setFilterFreq(int value)
{
    filterFreqKnob.setValue(
        value,
        juce::dontSendNotification
    );
}

void KeyGroupEditor::setFilterKeyFollow(int value)
{
    filterKeyFollowKnob.setValue(
        value,
        juce::dontSendNotification
    );
}
