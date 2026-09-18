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

    //lowNoteEditor.setColour(
    //    juce::TextEditor::textColourId,
    //    juce::Colour::fromRGB(35, 35, 35)
    //);

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

    addAndMakeVisible(filterEditAllToggle);

    filterEditAllToggle.setToggleState(
        false,
        juce::dontSendNotification
    );



    filterEditAllToggle.setToggleState(
        false,
        juce::dontSendNotification
    );


    env1AttackLabel.setText("Attack", juce::dontSendNotification);
    env1DecayLabel.setText("Decay", juce::dontSendNotification);
    env1SustainLabel.setText("Sustain", juce::dontSendNotification);
    env1ReleaseLabel.setText("Release", juce::dontSendNotification);


    lowNoteEditor.setInputRestrictions(3, "0123456789");
    highNoteEditor.setInputRestrictions(3, "0123456789");

    // =========================
// Numeric input restrictions
// =========================

// 0 - 99
    env1SustainEditor.setInputRestrictions(2, "0123456789");
    env1ReleaseEditor.setInputRestrictions(2, "0123456789");

    env2R1Editor.setInputRestrictions(2, "0123456789");
    env2L1Editor.setInputRestrictions(2, "0123456789");
    env2R2Editor.setInputRestrictions(2, "0123456789");
    env2L2Editor.setInputRestrictions(2, "0123456789");
    env2R3Editor.setInputRestrictions(2, "0123456789");
    env2L3Editor.setInputRestrictions(2, "0123456789");
    env2R4Editor.setInputRestrictions(2, "0123456789");
    env2L4Editor.setInputRestrictions(2, "0123456789");

    // signed numeric values
    tuneEditor.setInputRestrictions(3, "-0123456789");

    env2VelAttackEditor.setInputRestrictions(
        4, "-0123456789");

    env2VelReleaseEditor.setInputRestrictions(
        4, "-0123456789");

    env2NoteOffReleaseEditor.setInputRestrictions(
        4, "-0123456789");

    env2KeyTrackingEditor.setInputRestrictions(
        4, "-0123456789");

    velocityEnv2Editor.setInputRestrictions(
        4, "-0123456789");



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


    modulationSectionLabel.setFont(
        juce::Font(15.0f, juce::Font::bold)
    );

    keyXFadeSectionLabel.setFont(
        juce::Font(15.0f, juce::Font::bold)
    );



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



    addAndMakeVisible(velocityEnv2Label);
    addAndMakeVisible(velocityEnv2Editor);

    auto styleTextEditor =
        [](juce::TextEditor& editor)
        {
            editor.setColour(
                juce::TextEditor::textColourId,
                juce::Colour::fromRGB(35, 35, 35)
            );

            editor.setColour(
                juce::TextEditor::backgroundColourId,
                juce::Colour::fromRGB(218, 216, 208)
            );

            editor.setColour(
                juce::TextEditor::outlineColourId,
                juce::Colour::fromRGB(130, 130, 125)
            );
        };

    styleTextEditor(lowNoteEditor);
    styleTextEditor(highNoteEditor);
    styleTextEditor(tuneEditor);

    styleTextEditor(env1AttackEditor);
    styleTextEditor(env1DecayEditor);
    styleTextEditor(env1SustainEditor);
    styleTextEditor(env1ReleaseEditor);

    styleTextEditor(env2R1Editor);
    styleTextEditor(env2L1Editor);
    styleTextEditor(env2R2Editor);
    styleTextEditor(env2L2Editor);
    styleTextEditor(env2R3Editor);
    styleTextEditor(env2L3Editor);
    styleTextEditor(env2R4Editor);
    styleTextEditor(env2L4Editor);

    styleTextEditor(env2VelAttackEditor);
    styleTextEditor(env2VelReleaseEditor);
    styleTextEditor(velocityEnv2Editor);


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

                if (onRequestKeyboardFocus)
                    onRequestKeyboardFocus();
            };



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
            
            knob.setScrollWheelEnabled(false);

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
                juce::Colour::fromRGB(190, 45, 38)
            );

            knob.setColour(
                juce::Slider::rotarySliderOutlineColourId,
                juce::Colour(0xff26343a)
            );

            knob.setColour(
                juce::Slider::thumbColourId,
                juce::Colours::white
            );

            knob.setColour(
                juce::Slider::textBoxTextColourId,
                juce::Colour::fromRGB(35, 35, 35)
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


            if (!isTimerRunning())
            {
                startTimer(15);
            }
        };

    resonanceKnob.onDragEnd = [this]()
        {
            const int value =
                juce::roundToInt(resonanceKnob.getValue());

            pendingResonance = -1;



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

            if (onRequestKeyboardFocus)
                onRequestKeyboardFocus();
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



            if (!isTimerRunning())
            {
                startTimer(15);
            }
        };

    filterFreqKnob.onDragEnd = [this]()
        {
            const int value =
                juce::roundToInt(filterFreqKnob.getValue());

            pendingFilterFreq = -1;



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

            if (onRequestKeyboardFocus)
                onRequestKeyboardFocus();
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

    addAndMakeVisible(env2EditAllToggle);

    env2EditAllToggle.setToggleState(
        false,
        juce::dontSendNotification
    );



    env2EditAllToggle.onClick =
        [this]()
        {
            DBG(
                "ENV2 ALL TOGGLE CLICKED STATE="
                + juce::String(
                    env2EditAllToggle.getToggleState()
                    ? 1
                    : 0
                )
            );
        };

    addAndMakeVisible(env1EditAllToggle);

    env1EditAllToggle.setToggleState(
        false,
        juce::dontSendNotification
    );



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



    ePtchEditor.toFront(false);


    addAndMakeVisible(env2VelAttackLabel);
    addAndMakeVisible(env2VelAttackEditor);

    addAndMakeVisible(env2VelReleaseLabel);
    addAndMakeVisible(env2VelReleaseEditor);


    addAndMakeVisible(env1Editor);

    env1Editor.onEnvelopeChanged =
        [this](
            int attack,
            int decay,
            int sustain,
            int release)
        {
            const int oldAttack =
                currentKeygroup.env1.attack;

            const int oldDecay =
                currentKeygroup.env1.decay;

            const int oldSustain =
                currentKeygroup.env1.sustain;

            const int oldRelease =
                currentKeygroup.env1.release;

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

            if (attack != oldAttack &&
                onEnv1AttackChanged)
            {
                onEnv1AttackChanged(
                    currentKeygroupIndex,
                    attack
                );
            }

            if (decay != oldDecay &&
                onEnv1DecayChanged)
            {
                onEnv1DecayChanged(
                    currentKeygroupIndex,
                    decay
                );
            }

            if (sustain != oldSustain &&
                onEnv1SustainChanged)
            {
                onEnv1SustainChanged(
                    currentKeygroupIndex,
                    sustain
                );
            }

            if (release != oldRelease &&
                onEnv1ReleaseChanged)
            {
                onEnv1ReleaseChanged(
                    currentKeygroupIndex,
                    release
                );
            }
        };

    env1Editor.onEditFinished =
        [this]()
        {
            
            if (onRequestKeyboardFocus)
            {
           
                onRequestKeyboardFocus();
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

    {
        auto header =
            area.removeFromTop(24);

        filterEditAllToggle.setBounds(
            header.removeFromRight(85)
        );

        filterSectionLabel.setBounds(
            header
        );
    }


    {
        auto section =
            area.removeFromTop(130);

        constexpr int numberOfKnobs = 3;

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

        layoutKnob(
            resonanceArea,
            resonanceLabel,
            resonanceKnob
        );
    }

    {
        auto header =
            area.removeFromTop(24);

        env1EditAllToggle.setBounds(
            header.removeFromRight(85)
        );

        env1SectionLabel.setBounds(
            header
        );
    }


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



{
    auto header =
        area.removeFromTop(24);

    env2EditAllToggle.setBounds(
        header.removeFromRight(85)
    );

    env2SectionLabel.setBounds(
        header
    );
}

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

    auto velEnv2Area =
        row3.removeFromLeft(itemWidth);

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

    velocityEnv2Label.setBounds(
        velEnv2Area.removeFromLeft(envLabelWidth)
    );

    velocityEnv2Editor.setBounds(
        velEnv2Area.removeFromLeft(envEditorWidth)
    );

    section.removeFromTop(10);

    env2Editor.setBounds(
        section.removeFromTop(180)
    );
}

area.removeFromTop(6);



area.removeFromTop(6);

area.removeFromTop(6);





}


void KeyGroupEditor::paint(
    juce::Graphics& g)
{
    // S3000XL-style panel background
    g.fillAll(
        juce::Colour::fromRGB(
            198, 197, 191
        )
    );

    // Section divider lines
    g.setColour(
        juce::Colour::fromRGB(
            120, 119, 115
        ).withAlpha(0.45f)
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

void KeyGroupEditor::suspendFilterRealtime()
{
    pendingFilterFreq = -1;
    pendingResonance = -1;
}
