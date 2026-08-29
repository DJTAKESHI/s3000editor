#include "ProgramEditor.h"

static juce::String programNoteName(
    int note)
{
    static const char* noteNames[] =
    {
        "C", "C#", "D", "D#",
        "E", "F", "F#", "G",
        "G#", "A", "A#", "B"
    };

    note = juce::jlimit(
        0,
        127,
        note
    );

    const int noteIndex =
        note % 12;

    // S3000XL:
    // 24 = C0
    // 36 = C1
    // 96 = C6
    const int octave =
        note / 12 - 2;

    return
        juce::String(noteNames[noteIndex])
        + juce::String(octave);
}


ProgramEditor::ProgramEditor()
{
    titleLabel.setText(
        "Program Header",
        juce::dontSendNotification
    );

    addAndMakeVisible(titleLabel);

    auto setupSectionLabel =
        [this](
            juce::Label& label,
            const juce::String& text)
        {
            label.setText(
                text,
                juce::dontSendNotification
            );

            label.setColour(
                juce::Label::textColourId,
                juce::Colours::lightgrey
            );

            label.setFont(
                juce::Font(13.0f).boldened()
            );

            addAndMakeVisible(label);
        };

    setupSectionLabel(
        generalSectionLabel,
        "GENERAL"
    );

    setupSectionLabel(
        outputSectionLabel,
        "OUTPUT"
    );

    setupSectionLabel(
        pitchVoiceSectionLabel,
        "PITCH / VOICE"
    );

    setupSectionLabel(
        lfo1SectionLabel,
        "LFO1"
    );

    lfo1RateLabel.setText(
        "Rate",
        juce::dontSendNotification
    );

    addAndMakeVisible(lfo1RateLabel);

    lfo1RateSlider.setRange(
        0,
        99,
        1
    );

    lfo1RateSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );

    lfo1RateSlider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );

    addAndMakeVisible(lfo1RateSlider);

    lfo1RateValueLabel.setJustificationType(
        juce::Justification::centred
    );

    addAndMakeVisible(lfo1RateValueLabel);

    lfo1RateSlider.onValueChange =
        [this]()
        {
            lfo1RateValueLabel.setText(
                juce::String(
                    (int)lfo1RateSlider.getValue()
                ),
                juce::dontSendNotification
            );
        };

    lfo1RateSlider.onDragEnd =
        [this]()
        {
            currentProgram.lfo1Rate =
                (int)lfo1RateSlider.getValue();

            DBG(
                "PROGRAM LFO1 RATE EDIT FINISHED="
                + juce::String(
                    currentProgram.lfo1Rate
                )
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };


    auto setupRow =
        [this](
            juce::Label& label,
            juce::TextEditor& editor,
            const juce::String& text
            )
        {
            label.setText(
                text,
                juce::dontSendNotification
            );

            addAndMakeVisible(label);
            addAndMakeVisible(editor);

            // ‚Ü‚¸‚Íread-only
            editor.setReadOnly(true);
        };

    midiChannelLabel.setText(
        "MIDI Channel",
        juce::dontSendNotification
    );

    addAndMakeVisible(midiChannelLabel);

    midiChannelCombo.addItem(
        "OMNI",
        1
    );

    for (int ch = 1; ch <= 16; ++ch)
    {
        midiChannelCombo.addItem(
            juce::String(ch),
            ch + 1
        );
    }

    addAndMakeVisible(midiChannelCombo);

    midiChannelCombo.onChange =
        [this]()
        {
            const int selectedId =
                midiChannelCombo.getSelectedId();

            if (selectedId <= 0)
                return;

            if (selectedId == 1)
            {
                currentProgram.midiChannel =
                    255;
            }
            else
            {
                currentProgram.midiChannel =
                    selectedId - 2;
            }

            DBG(
                "PROGRAM MIDI CHANNEL CHANGED RAW="
                + juce::String(
                    currentProgram.midiChannel
                )
            );

            if (onProgramChanged)
            {
                onProgramChanged(
                    currentProgram
                );
            }
        };

    polyphonyLabel.setText(
        "Polyphony",
        juce::dontSendNotification
    );

    addAndMakeVisible(polyphonyLabel);

    for (int value = 1; value <= 32; ++value)
    {
        polyphonyCombo.addItem(
            juce::String(value),
            value
        );
    }

    tuneLabel.setText(
        "Program Tune",
        juce::dontSendNotification
    );

    addAndMakeVisible(tuneLabel);

    tuneSlider.setRange(
        -50.0,
        50.0,
        0.01
    );

    tuneSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );

    tuneSlider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );

    addAndMakeVisible(tuneSlider);

    tuneValueLabel.setJustificationType(
        juce::Justification::centred
    );

    addAndMakeVisible(tuneValueLabel);

    tuneSlider.onValueChange =
        [this]()
        {
            tuneValueLabel.setText(
                juce::String(
                    tuneSlider.getValue(),
                    2
                ),
                juce::dontSendNotification
            );
        };

    tuneSlider.onDragEnd =
        [this]()
        {
            currentProgram.tune =
                tuneSlider.getValue();

            DBG(
                "PROGRAM TUNE EDIT FINISHED TUNE="
                + juce::String(
                    currentProgram.tune,
                    2
                )
            );

            if (onProgramChanged)
            {
                onProgramChanged(
                    currentProgram
                );
            }
        };

    addAndMakeVisible(polyphonyCombo);

    polyphonyCombo.onChange =
        [this]()
        {
            const int selectedId =
                polyphonyCombo.getSelectedId();

            if (selectedId <= 0)
                return;

            currentProgram.polyphony =
                selectedId - 1;

            DBG(
                "PROGRAM POLYPHONY CHANGED RAW="
                + juce::String(
                    currentProgram.polyphony
                )
            );

            if (onProgramChanged)
            {
                onProgramChanged(
                    currentProgram
                );
            }
        };

    priorityLabel.setText(
        "Priority",
        juce::dontSendNotification
    );

    addAndMakeVisible(priorityLabel);

    priorityCombo.addItem("LOW", 1);
    priorityCombo.addItem("NORM", 2);
    priorityCombo.addItem("HIGH", 3);
    priorityCombo.addItem("HOLD", 4);

    addAndMakeVisible(priorityCombo);

    priorityCombo.onChange =
        [this]()
        {
            const int selectedId =
                priorityCombo.getSelectedId();

            if (selectedId <= 0)
                return;

            currentProgram.priority =
                selectedId - 1;

            DBG(
                "PROGRAM PRIORITY CHANGED RAW="
                + juce::String(
                    currentProgram.priority
                )
            );

            if (onProgramChanged)
            {
                onProgramChanged(
                    currentProgram
                );
            }
        };

    setupRow(
        playLowLabel,
        playLowEditor,
        "Play Low"
    );

    setupRow(
        playHighLabel,
        playHighEditor,
        "Play High"
    );
    outLevelLabel.setText(
        "Out Level",
        juce::dontSendNotification
    );

    addAndMakeVisible(outLevelLabel);

    outLevelSlider.setRange(
        0.0,
        99.0,
        1.0
    );

    outLevelSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );

    outLevelSlider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );

    addAndMakeVisible(outLevelSlider);

    outLevelValueLabel.setJustificationType(
        juce::Justification::centred
    );

    addAndMakeVisible(outLevelValueLabel);

    outLevelSlider.onValueChange =
        [this]()
        {
            const int level =
                static_cast<int>(
                    outLevelSlider.getValue()
                    );

            outLevelValueLabel.setText(
                juce::String(level),
                juce::dontSendNotification
            );
        };

    outLevelSlider.onDragEnd =
        [this]()
        {
            const int level =
                static_cast<int>(
                    outLevelSlider.getValue()
                    );

            currentProgram.individualOutputLevel =
                level;

            DBG(
                "PROGRAM OUT LEVEL EDIT FINISHED LEVEL="
                + juce::String(level)
            );

            if (onProgramChanged)
            {
                onProgramChanged(
                    currentProgram
                );
            }
        };



    addAndMakeVisible(playRangeBar);

    playRangeBar.onRangeChanged =
        [this](
            int low,
            int high)
        {
            currentProgram.playLow =
                low;

            currentProgram.playHigh =
                high;

            playLowEditor.setText(
                programNoteName(low),
                false
            );

            playHighEditor.setText(
                programNoteName(high),
                false
            );

            DBG(
                "PROGRAM RANGE CHANGED LOW="
                + juce::String(low)
                + " HIGH="
                + juce::String(high)
            );

            //if (onProgramChanged)
            //{
            //    onProgramChanged(
            //        currentProgram
            //    );
            //}

            playRangeBar.onEditFinished =
                [this](
                    int low,
                    int high)
                {
                    currentProgram.playLow =
                        low;

                    currentProgram.playHigh =
                        high;

                    DBG(
                        "PROGRAM RANGE EDIT FINISHED LOW="
                        + juce::String(low)
                        + " HIGH="
                        + juce::String(high)
                    );

                    if (onProgramChanged)
                    {
                        onProgramChanged(
                            currentProgram
                        );
                    }
                };

        };

    outputLabel.setText(
        "Out",
        juce::dontSendNotification
    );

    addAndMakeVisible(outputLabel);

    outputCombo.addItem(
        "OFF",
        1
    );

    for (int i = 1; i <= 8; ++i)
    {
        outputCombo.addItem(
            juce::String(i),
            i + 1
        );
    }

    addAndMakeVisible(outputCombo);

    outputCombo.onChange =
        [this]()
        {
            const int selectedId =
                outputCombo.getSelectedId();

            if (selectedId <= 0)
                return;

            if (selectedId == 1)
            {
                currentProgram.output = 255;
            }
            else
            {
                currentProgram.output =
                    selectedId - 2;
            }

            DBG(
                "PROGRAM OUT CHANGED RAW="
                + juce::String(
                    currentProgram.output
                )
            );

            if (onProgramChanged)
            {
                onProgramChanged(
                    currentProgram
                );
            }
        };

    stereoLabel.setText(
        "Stereo Level",
        juce::dontSendNotification
    );

    addAndMakeVisible(stereoLabel);

    stereoSlider.setRange(
        0.0,
        99.0,
        1.0
    );

    stereoSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );

    stereoSlider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );

    addAndMakeVisible(stereoSlider);

    stereoValueLabel.setJustificationType(
        juce::Justification::centred
    );

    addAndMakeVisible(stereoValueLabel);

    stereoSlider.onValueChange =
        [this]()
        {
            const int stereo =
                static_cast<int>(
                    stereoSlider.getValue()
                    );

            stereoValueLabel.setText(
                juce::String(stereo),
                juce::dontSendNotification
            );
        };

    stereoSlider.onDragEnd =
        [this]()
        {
            const int stereo =
                static_cast<int>(
                    stereoSlider.getValue()
                    );

            currentProgram.stereoLevel =
                stereo;

            DBG(
                "PROGRAM STEREO EDIT FINISHED STEREO="
                + juce::String(stereo)
            );

            if (onProgramChanged)
            {
                onProgramChanged(
                    currentProgram
                );
            }
        };

    panLabel.setText(
        "Pan",
        juce::dontSendNotification
    );

    loudnessLabel.setText(
        "Loudness",
        juce::dontSendNotification
    );

    addAndMakeVisible(loudnessLabel);

    loudnessSlider.setRange(
        0.0,
        99.0,
        1.0
    );

    loudnessSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );

    loudnessSlider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );

    addAndMakeVisible(loudnessSlider);

    loudnessValueLabel.setJustificationType(
        juce::Justification::centred
    );

    loudnessSlider.onValueChange =
        [this]()
        {
            const int loudness =
                static_cast<int>(
                    loudnessSlider.getValue()
                    );

            loudnessValueLabel.setText(
                juce::String(loudness),
                juce::dontSendNotification
            );
        };

    loudnessSlider.onDragEnd =
        [this]()
        {
            const int loudness =
                static_cast<int>(
                    loudnessSlider.getValue()
                    );

            currentProgram.loudness =
                loudness;

            DBG(
                "PROGRAM LOUDNESS EDIT FINISHED LOUDNESS="
                + juce::String(loudness)
            );

            if (onProgramChanged)
            {
                onProgramChanged(
                    currentProgram
                );
            }
        };


    addAndMakeVisible(loudnessValueLabel);

    addAndMakeVisible(panLabel);

    panSlider.setRange(
        -50.0,
        50.0,
        1.0
    );

    panSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );

    panSlider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );

    addAndMakeVisible(panSlider);

    panValueLabel.setJustificationType(
        juce::Justification::centred
    );

    addAndMakeVisible(panValueLabel);

    panSlider.onValueChange =
        [this]()
        {
            const int pan =
                static_cast<int>(
                    panSlider.getValue()
                    );

            juce::String text;

            if (pan < 0)
            {
                text =
                    "L"
                    + juce::String(
                        std::abs(pan)
                    );
            }
            else if (pan > 0)
            {
                text =
                    "R"
                    + juce::String(pan);
            }
            else
            {
                text = "MID";
            }

            panValueLabel.setText(
                text,
                juce::dontSendNotification
            );
        };

    panSlider.onDragEnd =
        [this]()
        {
            const int pan =
                static_cast<int>(
                    panSlider.getValue()
                    );

            currentProgram.pan =
                pan;

            DBG(
                "PROGRAM PAN EDIT FINISHED PAN="
                + juce::String(pan)
            );

            if (onProgramChanged)
            {
                onProgramChanged(
                    currentProgram
                );
            }
        };

    pressurePitchLabel.setText(
        "Pressure -> Pitch",
        juce::dontSendNotification
    );

    addAndMakeVisible(pressurePitchLabel);

    pressurePitchSlider.setRange(
        0,
        12,
        1
    );

    pressurePitchSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );

    pressurePitchSlider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );

    addAndMakeVisible(pressurePitchSlider);

    pressurePitchValueLabel.setJustificationType(
        juce::Justification::centred
    );

    addAndMakeVisible(pressurePitchValueLabel);

    pressurePitchSlider.onValueChange =
        [this]()
        {
            pressurePitchValueLabel.setText(
                juce::String(
                    (int)pressurePitchSlider.getValue()
                ),
                juce::dontSendNotification
            );
        };

    pressurePitchSlider.onDragEnd =
        [this]()
        {
            currentProgram.pressurePitch =
                (int)pressurePitchSlider.getValue();

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    bendDownLabel.setText(
        "Bend Down",
        juce::dontSendNotification
    );

    addAndMakeVisible(bendDownLabel);

    bendDownSlider.setRange(
        0,
        24,
        1
    );

    bendDownSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );

    bendDownSlider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );

    addAndMakeVisible(bendDownSlider);

    bendDownValueLabel.setJustificationType(
        juce::Justification::centred
    );

    addAndMakeVisible(bendDownValueLabel);

    bendDownSlider.onValueChange =
        [this]()
        {
            bendDownValueLabel.setText(
                juce::String(
                    (int)bendDownSlider.getValue()
                ),
                juce::dontSendNotification
            );
        };

    bendDownSlider.onDragEnd =
        [this]()
        {
            currentProgram.bendDown =
                (int)bendDownSlider.getValue();

            DBG(
                "PROGRAM BEND DOWN EDIT FINISHED="
                + juce::String(
                    currentProgram.bendDown
                )
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    bendModeLabel.setText(
        "Bend Mode",
        juce::dontSendNotification
    );

    addAndMakeVisible(bendModeLabel);

    bendModeCombo.addItem("NORMAL", 1);
    bendModeCombo.addItem("HELD", 2);

    addAndMakeVisible(bendModeCombo);

    bendModeCombo.onChange =
        [this]()
        {
            const int selectedId =
                bendModeCombo.getSelectedId();

            if (selectedId <= 0)
                return;

            currentProgram.bendMode =
                selectedId - 1;

            DBG(
                "PROGRAM BEND MODE CHANGED RAW="
                + juce::String(
                    currentProgram.bendMode
                )
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    transposeLabel.setText(
        "Transpose",
        juce::dontSendNotification
    );

    addAndMakeVisible(transposeLabel);

    transposeSlider.setRange(
        -50,
        50,
        1
    );

    transposeSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );

    transposeSlider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );

    addAndMakeVisible(transposeSlider);

    transposeValueLabel.setJustificationType(
        juce::Justification::centred
    );

    addAndMakeVisible(transposeValueLabel);

    transposeSlider.onValueChange =
        [this]()
        {
            const int value =
                (int)transposeSlider.getValue();

            juce::String text;

            if (value > 0)
                text = "+" + juce::String(value);
            else
                text = juce::String(value);

            transposeValueLabel.setText(
                text,
                juce::dontSendNotification
            );
        };

    transposeSlider.onDragEnd =
        [this]()
        {
            currentProgram.transpose =
                (int)transposeSlider.getValue();

            DBG(
                "PROGRAM TRANSPOSE EDIT FINISHED="
                + juce::String(currentProgram.transpose)
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };


    legatoLabel.setText(
        "Legato",
        juce::dontSendNotification
    );

    addAndMakeVisible(legatoLabel);

    legatoCombo.addItem("OFF", 1);
    legatoCombo.addItem("ON", 2);

    addAndMakeVisible(legatoCombo);

    legatoCombo.onChange =
        [this]()
        {
            const int selectedId =
                legatoCombo.getSelectedId();

            if (selectedId <= 0)
                return;

            currentProgram.legato =
                (selectedId == 2);

            DBG(
                "PROGRAM LEGATO CHANGED="
                + juce::String(
                    currentProgram.legato
                    ? "ON"
                    : "OFF"
                )
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    voiceAssignLabel.setText(
        "Voice Steal",
        juce::dontSendNotification
    );

    addAndMakeVisible(voiceAssignLabel);

    voiceAssignCombo.addItem("OLDEST", 1);
    voiceAssignCombo.addItem("QUIETEST", 2);

    addAndMakeVisible(voiceAssignCombo);

    voiceAssignCombo.onChange =
        [this]()
        {
            const int selectedId =
                voiceAssignCombo.getSelectedId();

            if (selectedId <= 0)
                return;

            // VASSOQ:
            // 0 = OLDEST
            // 1 = QUIETEST
            currentProgram.voiceAssign =
                selectedId - 1;

            DBG(
                "PROGRAM VOICE STEAL CHANGED RAW="
                + juce::String(
                    currentProgram.voiceAssign
                )
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    lfo1DesyncLabel.setText(
        "LFO1 Desync",
        juce::dontSendNotification
    );

    addAndMakeVisible(lfo1DesyncLabel);

    lfo1DesyncCombo.addItem("OFF", 1);
    lfo1DesyncCombo.addItem("ON", 2);

    addAndMakeVisible(lfo1DesyncCombo);

    lfo1DesyncCombo.onChange =
        [this]()
        {
            const int selectedId =
                lfo1DesyncCombo.getSelectedId();

            if (selectedId <= 0)
                return;

            currentProgram.lfo1Desync =
                (selectedId == 2);

            DBG(
                "PROGRAM LFO1 DESYNC CHANGED="
                + juce::String(
                    currentProgram.lfo1Desync
                    ? "ON"
                    : "OFF"
                )
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };



}

void ProgramEditor::paint(
    juce::Graphics& g)
{
    auto drawCard =
        [&g](
            const juce::Rectangle<int>& bounds)
        {
            g.setColour(
                juce::Colours::white
                .withAlpha(0.05f)
            );

            g.fillRoundedRectangle(
                bounds.toFloat(),
                6.0f
            );

            g.setColour(
                juce::Colours::white
                .withAlpha(0.10f)
            );

            g.drawRoundedRectangle(
                bounds.toFloat(),
                6.0f,
                1.0f
            );
        };

    drawCard(generalCardBounds);
    drawCard(outputCardBounds);
    drawCard(pitchVoiceCardBounds);
    drawCard(lfo1CardBounds);
}


void ProgramEditor::setProgram(
    const Program& program)
{
    currentProgram = program;

    // raw 0 = MIDI CH 1
    if (program.midiChannel == 255)
    {
        midiChannelCombo.setSelectedId(
            1,
            juce::dontSendNotification
        );
    }
    else if (
        program.midiChannel >= 0
        && program.midiChannel <= 15
        )
    {
        midiChannelCombo.setSelectedId(
            program.midiChannel + 2,
            juce::dontSendNotification
        );
    }
    else
    {
        midiChannelCombo.setSelectedId(
            0,
            juce::dontSendNotification
        );
    }

    pressurePitchSlider.setValue(
        program.pressurePitch,
        juce::dontSendNotification
    );

    pressurePitchValueLabel.setText(
        juce::String(program.pressurePitch),
        juce::dontSendNotification
    );

    polyphonyCombo.setSelectedId(
        program.polyphony + 1,
        juce::dontSendNotification
    );

    juce::String priorityText;

    switch (program.priority)
    {
    case 0:
        priorityText = "LOW";
        break;

    case 1:
        priorityText = "NORMAL";
        break;

    case 2:
        priorityText = "HIGH";
        break;

    default:
        priorityText =
            juce::String(program.priority);
        break;
    }

    if (
        program.priority >= 0
        && program.priority <= 3
        )
    {
        priorityCombo.setSelectedId(
            program.priority + 1,
            juce::dontSendNotification
        );
    }
    else
    {
        priorityCombo.setSelectedId(
            0,
            juce::dontSendNotification
        );
    }

    playLowEditor.setText(
        programNoteName(program.playLow),
        false
    );

    playHighEditor.setText(
        programNoteName(program.playHigh),
        false
    );

    playRangeBar.setRange(
        program.playLow,
        program.playHigh
    );

    outLevelSlider.setValue(
        program.individualOutputLevel,
        juce::dontSendNotification
    );

    outLevelValueLabel.setText(
        juce::String(
            program.individualOutputLevel
        ),
        juce::dontSendNotification
    );


    if (program.output == 255)
    {
        outputCombo.setSelectedId(
            1,
            juce::dontSendNotification
        );
    }
    else if (
        program.output >= 0
        && program.output <= 7
        )
    {
        outputCombo.setSelectedId(
            program.output + 2,
            juce::dontSendNotification
        );
    }
    else
    {
        outputCombo.setSelectedId(
            0,
            juce::dontSendNotification
        );
    }

    stereoSlider.setValue(
        program.stereoLevel,
        juce::dontSendNotification
    );

    stereoValueLabel.setText(
        juce::String(program.stereoLevel),
        juce::dontSendNotification
    );

    juce::String panText;

    if (program.pan < 0)
    {
        panText =
            "L"
            + juce::String(
                std::abs(program.pan)
            );
    }
    else if (program.pan > 0)
    {
        panText =
            "R"
            + juce::String(program.pan);
    }
    else
    {
        panText = "MID";
    }

    panSlider.setValue(
        program.pan,
        juce::dontSendNotification
    );

    panValueLabel.setText(
        panText,
        juce::dontSendNotification
    );

    loudnessSlider.setValue(
        program.loudness,
        juce::dontSendNotification
    );

    loudnessValueLabel.setText(
        juce::String(program.loudness),
        juce::dontSendNotification
    );

    tuneSlider.setValue(
        program.tune,
        juce::dontSendNotification
    );

    tuneValueLabel.setText(
        juce::String(program.tune, 2),
        juce::dontSendNotification
    );

    bendUpSlider.setValue(
        program.bendUp,
        juce::dontSendNotification
    );

    bendUpValueLabel.setText(
        juce::String(program.bendUp),
        juce::dontSendNotification
    );

    bendDownSlider.setValue(
        program.bendDown,
        juce::dontSendNotification
    );

    bendDownValueLabel.setText(
        juce::String(program.bendDown),
        juce::dontSendNotification
    );

    bendModeCombo.setSelectedId(
        program.bendMode + 1,
        juce::dontSendNotification
    );

    transposeSlider.setValue(
        program.transpose,
        juce::dontSendNotification
    );

    const auto transposeText =
        program.transpose > 0
        ? "+" + juce::String(program.transpose)
        : juce::String(program.transpose);

    transposeValueLabel.setText(
        transposeText,
        juce::dontSendNotification
    );

    legatoCombo.setSelectedId(
        program.legato ? 2 : 1,
        juce::dontSendNotification
    );

    voiceAssignCombo.setSelectedId(
        program.voiceAssign + 1,
        juce::dontSendNotification
    );

    lfo1DesyncCombo.setSelectedId(
        program.lfo1Desync ? 2 : 1,
        juce::dontSendNotification
    );

    lfo1RateSlider.setValue(
        program.lfo1Rate,
        juce::dontSendNotification
    );

    lfo1RateValueLabel.setText(
        juce::String(program.lfo1Rate),
        juce::dontSendNotification

    );

    DBG("PROGRAM EDITOR UPDATED");
}

void ProgramEditor::resized()
{
    auto area =
        getLocalBounds().reduced(10);

    titleLabel.setBounds(
        area.removeFromTop(32)
    );

    area.removeFromTop(6);

    const int gap = 10;

    auto left =
        area.removeFromLeft(
            (area.getWidth() - gap) / 2
        );

    area.removeFromLeft(gap);

    auto right = area;

    // =========================
    // GENERAL
    // =========================

    generalCardBounds =
        left.removeFromTop(260);

    auto general =
        generalCardBounds.reduced(10);

    generalSectionLabel.setBounds(
        general.removeFromTop(24)
    );

    constexpr int rowHeight = 28;
    constexpr int labelWidth = 105;

    auto addRow =
        [=](
            juce::Rectangle<int>& section,
            juce::Label& label,
            juce::TextEditor& editor)
        {
            auto row =
                section.removeFromTop(rowHeight);

            label.setBounds(
                row.removeFromLeft(labelWidth)
            );

            editor.setBounds(
                row.reduced(2)
            );
        };

    {
        auto row =
            general.removeFromTop(rowHeight);

        midiChannelLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        midiChannelCombo.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row =
            general.removeFromTop(rowHeight);

        polyphonyLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        polyphonyCombo.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row =
            general.removeFromTop(rowHeight);

        priorityLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        priorityCombo.setBounds(
            row.reduced(2)
        );
    }

    addRow(
        general,
        playLowLabel,
        playLowEditor
    );

    addRow(
        general,
        playHighLabel,
        playHighEditor
    );

    general.removeFromTop(6);

    playRangeBar.setBounds(
        general.removeFromTop(60)
    );

    left.removeFromTop(10);

    // =========================
    // OUTPUT
    // =========================

    outputCardBounds =
        right.removeFromTop(182);

    auto output =
        outputCardBounds.reduced(10);

    outputSectionLabel.setBounds(
        output.removeFromTop(24)
    );

    {
        auto row =
            output.removeFromTop(rowHeight);

        outputLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        outputCombo.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row =
            output.removeFromTop(rowHeight);

        outLevelLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        outLevelValueLabel.setBounds(
            row.removeFromRight(45)
        );

        outLevelSlider.setBounds(
            row.reduced(2)
        );
    }



    {
        auto row =
            output.removeFromTop(rowHeight);

        stereoLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        stereoValueLabel.setBounds(
            row.removeFromRight(45)
        );

        stereoSlider.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row =
            output.removeFromTop(rowHeight);

        panLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        panValueLabel.setBounds(
            row.removeFromRight(45)
        );

        panSlider.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row =
            output.removeFromTop(rowHeight);

        loudnessLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        loudnessValueLabel.setBounds(
            row.removeFromRight(45)
        );

        loudnessSlider.setBounds(
            row.reduced(2)
        );
    }

    right.removeFromTop(10);

    // =========================
// PITCH / VOICE
// =========================

    pitchVoiceCardBounds =
        left.removeFromTop(310);

    auto pitchVoice =
        pitchVoiceCardBounds.reduced(10);

    pitchVoiceSectionLabel.setBounds(
        pitchVoice.removeFromTop(24)
    );

    {
        auto row =
            pitchVoice.removeFromTop(rowHeight);

        tuneLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        tuneValueLabel.setBounds(
            row.removeFromRight(55)
        );

        tuneSlider.setBounds(
            row.reduced(2)
        );
    }

    


    {
        auto row =
            pitchVoice.removeFromTop(rowHeight);

        bendUpLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        bendUpValueLabel.setBounds(
            row.removeFromRight(55)
        );

        bendUpSlider.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row =
            pitchVoice.removeFromTop(rowHeight);

        bendDownLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        bendDownValueLabel.setBounds(
            row.removeFromRight(55)
        );

        bendDownSlider.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row =
            pitchVoice.removeFromTop(rowHeight);

        bendModeLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        bendModeCombo.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row =
            pitchVoice.removeFromTop(rowHeight);

        transposeLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        transposeValueLabel.setBounds(
            row.removeFromRight(55)
        );

        transposeSlider.setBounds(
            row.reduced(2)
        );
    }


    {
        auto row =
            pitchVoice.removeFromTop(rowHeight);

        legatoLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        legatoCombo.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row =
            pitchVoice.removeFromTop(rowHeight);

        voiceAssignLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        voiceAssignCombo.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row =
            pitchVoice.removeFromTop(rowHeight);

        lfo1DesyncLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        lfo1DesyncCombo.setBounds(
            row.reduced(2)
        );
    }

    // =========================
    // LFO1
    // =========================

    lfo1CardBounds =
        right.removeFromTop(100);

    auto lfo1 =
        lfo1CardBounds.reduced(10);

    lfo1SectionLabel.setBounds(
        lfo1.removeFromTop(24)
    );

    {
        auto row =
            lfo1.removeFromTop(rowHeight);

        lfo1RateLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        lfo1RateValueLabel.setBounds(
            row.removeFromRight(55)
        );

        lfo1RateSlider.setBounds(
            row.reduced(2)
        );
    }




}