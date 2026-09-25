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

    DBG("ProgramEditor CONSTRUCTOR this="
        + juce::String::toHexString(
            reinterpret_cast<juce::pointer_sized_int>(this)));

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
                juce::Colour::fromRGB(55, 55, 52)
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
    
    lfo1RateSlider.setScrollWheelEnabled(false);

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

    DBG("UI LFO1 RATE="
        + juce::String(lfo1RateSlider.getValue()));

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

            // まずはread-only
            editor.setReadOnly(true);
        };

    programNameLabel.setText(
        "Program Name",
        juce::dontSendNotification
    );

    addAndMakeVisible(programNameLabel);
    addAndMakeVisible(programNameEditor);

    programNameEditor.setWantsKeyboardFocus(false);

    programNameEditor.onReturnKey =
        [this]()
        {
            auto name =
                programNameEditor.getText()
                .trim()
                .toUpperCase()
                .substring(0, 12);

            if (name.isEmpty())
            {
                programNameEditor.setText(
                    juce::String(currentProgram.name),
                    false
                );
                return;
            }

            programNameEditor.setText(
                name,
                false
            );

            currentProgram.name =
                name.toStdString();

            if (onProgramChanged)
                onProgramChanged(currentProgram);
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

    lfo1WaveCombo.addItem("Triangle", 1);
    lfo1WaveCombo.addItem("Sawtooth", 2);
    lfo1WaveCombo.addItem("Square", 3);
    lfo1WaveCombo.addItem("Random", 4);

    lfo1WaveCombo.setSelectedId(
        currentProgram.lfo1Wave + 1,
        juce::dontSendNotification
    );

    addAndMakeVisible(lfo1WaveCombo);

    lfo1WaveCombo.onChange =
        [this]()
        {
            const int selectedId =
                lfo1WaveCombo.getSelectedId();

            if (selectedId <= 0)
                return;

            currentProgram.lfo1Wave =
                selectedId - 1;

            DBG(
                "PROGRAM LFO1 WAVE CHANGED RAW="
                + juce::String(currentProgram.lfo1Wave)
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

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

//    tuneSlider.setDoubleClickReturnValue(
//        false,
//        0.0
//    );

    tuneSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );
    
    tuneSlider.setScrollWheelEnabled(false);

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
    


    tuneSlider.onDoubleClickReset =
        [this]()
        {
            currentProgram.tune =
                tuneSlider.getValue();

            DBG(
                "PROGRAM TUNE DOUBLE CLICK RESET TUNE="
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
    
    outLevelSlider.setScrollWheelEnabled(false);

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
        "Output Routing",
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
            "OUT " + juce::String(i),
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
    
    stereoSlider.setScrollWheelEnabled(false);

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
    
    panSlider.setScrollWheelEnabled(false);

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

    DBG("UI PAN="
        + juce::String(panSlider.getValue()));

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
    
    pressurePitchSlider.setScrollWheelEnabled(false);

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
    
    bendDownSlider.setScrollWheelEnabled(false);

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
    
    bendUpLabel.setText(
        "Bend Up",
        juce::dontSendNotification
    );
    
    addAndMakeVisible(bendUpLabel);
    
    bendUpSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );

    bendUpSlider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );

    bendUpSlider.setScrollWheelEnabled(false);
    
    addAndMakeVisible(bendUpSlider);
    addAndMakeVisible(bendUpValueLabel);

    bendUpSlider.onValueChange =
        [this]()
        {
            bendUpValueLabel.setText(
                juce::String(
                    (int)bendUpSlider.getValue()
                ),
                juce::dontSendNotification
            );
        };
    
    bendUpValueLabel.setJustificationType(
        juce::Justification::centredRight
    );

    bendDownValueLabel.setJustificationType(
        juce::Justification::centredRight
    );
    
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
    
    transposeSlider.setScrollWheelEnabled(false);

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

    lfo1DepthLabel.setText(
        "Depth",
        juce::dontSendNotification
    );

    addAndMakeVisible(lfo1DepthLabel);

    lfo1DepthSlider.setRange(
        0,
        99,
        1
    );
    
    lfo1DepthSlider.setScrollWheelEnabled(false);

    lfo1DepthSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );

    lfo1DepthSlider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );

    addAndMakeVisible(lfo1DepthSlider);

    lfo1DepthValueLabel.setJustificationType(
        juce::Justification::centred
    );

    addAndMakeVisible(lfo1DepthValueLabel);

    lfo1DepthSlider.onValueChange =
        [this]()
        {
            lfo1DepthValueLabel.setText(
                juce::String(
                    (int)lfo1DepthSlider.getValue()
                ),
                juce::dontSendNotification
            );
        };

    lfo1DepthSlider.onDragEnd =
        [this]()
        {
            currentProgram.lfo1Depth =
                (int)lfo1DepthSlider.getValue();

            DBG(
                "PROGRAM LFO1 DEPTH EDIT FINISHED="
                + juce::String(currentProgram.lfo1Depth)
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    lfo1DelayLabel.setText(
        "Delay",
        juce::dontSendNotification
    );

    addAndMakeVisible(lfo1DelayLabel);

    lfo1DelaySlider.setRange(
        0,
        99,
        1
    );
    
    lfo1DelaySlider.setScrollWheelEnabled(false);

    lfo1DelaySlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );

    lfo1DelaySlider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );

    addAndMakeVisible(lfo1DelaySlider);

    lfo1DelayValueLabel.setJustificationType(
        juce::Justification::centred
    );

    addAndMakeVisible(lfo1DelayValueLabel);

    lfo1DelaySlider.onValueChange =
        [this]()
        {
            lfo1DelayValueLabel.setText(
                juce::String(
                    (int)lfo1DelaySlider.getValue()
                ),
                juce::dontSendNotification
            );
        };

    lfo1DelaySlider.onDragEnd =
        [this]()
        {
            currentProgram.lfo1Delay =
                (int)lfo1DelaySlider.getValue();

            DBG(
                "PROGRAM LFO1 DELAY EDIT FINISHED="
                + juce::String(currentProgram.lfo1Delay)
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    setupSectionLabel(
        lfo2SectionLabel,
        "LFO2"
    );

    auto setupLfo2Slider =
        [this](
            juce::Label& label,
            juce::Slider& slider,
            juce::Label& valueLabel,
            const juce::String& name)
        {
            label.setText(
                name,
                juce::dontSendNotification
            );

            addAndMakeVisible(label);

            slider.setRange(0, 99, 1);
            
            slider.setScrollWheelEnabled(false);

            slider.setSliderStyle(
                juce::Slider::LinearHorizontal
            );

            slider.setTextBoxStyle(
                juce::Slider::NoTextBox,
                false,
                0,
                0
            );

            addAndMakeVisible(slider);

            valueLabel.setJustificationType(
                juce::Justification::centred
            );

            addAndMakeVisible(valueLabel);

            slider.onValueChange =
                [&slider, &valueLabel]()
                {
                    valueLabel.setText(
                        juce::String(
                            (int)slider.getValue()
                        ),
                        juce::dontSendNotification
                    );
                };
        };

    setupLfo2Slider(
        lfo2RateLabel,
        lfo2RateSlider,
        lfo2RateValueLabel,
        "Rate"
    );

    setupLfo2Slider(
        lfo2DepthLabel,
        lfo2DepthSlider,
        lfo2DepthValueLabel,
        "Depth"
    );

    setupLfo2Slider(
        lfo2DelayLabel,
        lfo2DelaySlider,
        lfo2DelayValueLabel,
        "Delay"
    );

    lfo2RateSlider.onDragEnd =
        [this]()
        {
            currentProgram.lfo2Rate =
                (int)lfo2RateSlider.getValue();

            DBG(
                "PROGRAM LFO2 RATE EDIT FINISHED="
                + juce::String(currentProgram.lfo2Rate)
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    lfo2DepthSlider.onDragEnd =
        [this]()
        {
            currentProgram.lfo2Depth =
                (int)lfo2DepthSlider.getValue();

            DBG(
                "PROGRAM LFO2 DEPTH EDIT FINISHED="
                + juce::String(currentProgram.lfo2Depth)
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    lfo2DelaySlider.onDragEnd =
        [this]()
        {
            currentProgram.lfo2Delay =
                (int)lfo2DelaySlider.getValue();

            DBG(
                "PROGRAM LFO2 DELAY EDIT FINISHED="
                + juce::String(currentProgram.lfo2Delay)
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    lfo2WaveLabel.setText(
        "Wave",
        juce::dontSendNotification
    );

    addAndMakeVisible(lfo2WaveLabel);

    lfo2WaveCombo.addItem("Triangle", 1);
    lfo2WaveCombo.addItem("Sawtooth", 2);
    lfo2WaveCombo.addItem("Square", 3);
    lfo2WaveCombo.addItem("Random", 4);

    addAndMakeVisible(lfo2WaveCombo);

    portamentoEnableToggle.onClick =
        [this]()
        {
            if (onPortamentoEnableChanged)
            {
                onPortamentoEnableChanged(
                    portamentoEnableToggle.getToggleState()
                );
            }
        };

    portamentoTypeCombo.onChange =
        [this]()
        {
            if (onPortamentoTypeChanged)
            {
                const int value =
                    portamentoTypeCombo.getSelectedId() - 1;

                onPortamentoTypeChanged(value);
            }
        };

    portamentoValueSlider.onValueChange =
        [this]()
        {
            if (onPortamentoValueChanged)
            {
                onPortamentoValueChanged(
                    static_cast<int>(
                        portamentoValueSlider.getValue()
                        )
                );
            }
        };


    lfo2WaveCombo.onChange =
        [this]()
        {
            const int selectedId =
                lfo2WaveCombo.getSelectedId();

            if (selectedId <= 0)
                return;

            currentProgram.lfo2Wave =
                selectedId - 1;

            DBG(
                "PROGRAM LFO2 WAVE CHANGED RAW="
                + juce::String(currentProgram.lfo2Wave)
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    setupSectionLabel(
        softPedalSectionLabel,
        "SOFT PEDAL"
    );

    auto setupSoftPedalSlider =
        [this](
            juce::Label& label,
            juce::Slider& slider,
            juce::Label& valueLabel,
            const juce::String& name)
        {
            label.setText(
                name,
                juce::dontSendNotification
            );

            addAndMakeVisible(label);

            slider.setRange(0, 99, 1);

            slider.setSliderStyle(
                juce::Slider::LinearHorizontal
            );
            
            slider.setScrollWheelEnabled(false);

            slider.setTextBoxStyle(
                juce::Slider::NoTextBox,
                false,
                0,
                0
            );

            addAndMakeVisible(slider);

            valueLabel.setJustificationType(
                juce::Justification::centred
            );

            addAndMakeVisible(valueLabel);

            slider.onValueChange =
                [&slider, &valueLabel]()
                {
                    valueLabel.setText(
                        juce::String(
                            (int)slider.getValue()
                        ),
                        juce::dontSendNotification
                    );
                };
        };

    //setupSoftPedalSlider(
    //    softLoudnessLabel,
    //    softLoudnessSlider,
    //    softLoudnessValueLabel,
    //    "Loudness"
    //);

    //setupSoftPedalSlider(
    //    softAttackLabel,
    //    softAttackSlider,
    //    softAttackValueLabel,
    //    "Attack"
    //);

    //setupSoftPedalSlider(
    //    softFilterLabel,
    //    softFilterSlider,
    //    softFilterValueLabel,
    //    "Filter"
    //);

    softLoudnessSlider.onDragEnd =
        [this]()
        {
            currentProgram.softLoudness =
                (int)softLoudnessSlider.getValue();

            DBG(
                "PROGRAM SOFT LOUDNESS EDIT FINISHED="
                + juce::String(currentProgram.softLoudness)
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    softAttackSlider.onDragEnd =
        [this]()
        {
            currentProgram.softAttack =
                (int)softAttackSlider.getValue();

            DBG(
                "PROGRAM SOFT ATTACK EDIT FINISHED="
                + juce::String(currentProgram.softAttack)
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    softFilterSlider.onDragEnd =
        [this]()
        {
            currentProgram.softFilter =
                (int)softFilterSlider.getValue();

            DBG(
                "PROGRAM SOFT FILTER EDIT FINISHED="
                + juce::String(currentProgram.softFilter)
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };


    setupSectionLabel(
        modPanSectionLabel,
        "MODULATION - PAN"
    );

    setupSectionLabel(
        modAmpSectionLabel,
        "MODULATION - LOUDNESS"
    );

    setupSectionLabel(
        modLfo1SectionLabel,
        "MODULATION - LFO1"
    );

    setupSectionLabel(
        modFilterPitchSectionLabel,
        "MODULATION - FILTER / PITCH"
    );

    auto setupModSourceCombo =
        [this](
            juce::Label& label,
            juce::ComboBox& combo,
            const juce::String& name)
        {
            label.setText(
                name,
                juce::dontSendNotification
            );

            addAndMakeVisible(label);

            combo.addItem("No Source", 1);
            combo.addItem("Modwheel", 2);
            combo.addItem("Bend", 3);
            combo.addItem("Pressure", 4);
            combo.addItem("External", 5);
            combo.addItem("Note-on velocity", 6);
            combo.addItem("Key", 7);
            combo.addItem("LFO1", 8);
            combo.addItem("LFO2", 9);
            combo.addItem("Env1", 10);
            combo.addItem("Env2", 11);
            combo.addItem("!Modwheel", 12);
            combo.addItem("!Bend", 13);
            combo.addItem("!External", 14);
            //combo.addItem("Env3", 15);

            addAndMakeVisible(combo);
        };

    setupModSourceCombo(
        modPan1Label,
        modPan1SourceCombo,
        "Source 1"
    );

    setupModSourceCombo(
        modPan2Label,
        modPan2SourceCombo,
        "Source 2"
    );

    setupModSourceCombo(
        modPan3Label,
        modPan3SourceCombo,
        "Source 3"
    );

    setupModSourceCombo(
        modAmp1Label,
        modAmp1SourceCombo,
        "Source 1"
    );

    setupModSourceCombo(
        modAmp2Label,
        modAmp2SourceCombo,
        "Source 2"
    );

//    setupModSourceCombo(
//        modAmp3Label,
//        modAmp3SourceCombo,
//        "Source 3"
//    );

    setupModSourceCombo(
        modLfo1RateLabel,
        modLfo1RateSourceCombo,
        "Rate"
    );

    setupModSourceCombo(
        modLfo1DepthLabel,
        modLfo1DepthSourceCombo,
        "Depth"
    );

    setupModSourceCombo(
        modLfo1DelayLabel,
        modLfo1DelaySourceCombo,
        "Delay"
    );

    setupModSourceCombo(
        modFilter1Label,
        modFilter1SourceCombo,
        "Filter 1"
    );

    setupModSourceCombo(
        modFilter2Label,
        modFilter2SourceCombo,
        "Filter 2"
    );

    setupModSourceCombo(
        modFilter3Label,
        modFilter3SourceCombo,
        "Filter 3"
    );

    setupModSourceCombo(
        modPitchLabel,
        modPitchSourceCombo,
        "Pitch"
    );

    auto setupModAmountSlider =
        [this](
            juce::Slider& slider,
            juce::Label& valueLabel)
        {
            slider.setRange(
                -50,
                50,
                1
            );

            slider.setSliderStyle(
                juce::Slider::LinearHorizontal
            );
            
            slider.setScrollWheelEnabled(false);

            slider.setTextBoxStyle(
                juce::Slider::NoTextBox,
                false,
                0,
                0
            );

            addAndMakeVisible(slider);

            valueLabel.setJustificationType(
                juce::Justification::centred
            );

            addAndMakeVisible(valueLabel);

            slider.onValueChange =
                [&slider, &valueLabel]()
                {
                    const int value =
                        (int)slider.getValue();

                    valueLabel.setText(
                        value > 0
                        ? "+" + juce::String(value)
                        : juce::String(value),
                        juce::dontSendNotification
                    );
                };
        };

    setupModAmountSlider(
        modPan1AmountSlider,
        modPan1AmountLabel
    );

    lfo1PitchLabel.setText(
        "LFO1 -> Pitch",
        juce::dontSendNotification
    );

    addAndMakeVisible(lfo1PitchLabel);

    setupModAmountSlider(
        lfo1PitchSlider,
        lfo1PitchAmountLabel
    );

    //env2EnvelopePitchLabel.setText(
    //    "Env2 -> Pitch",
    //    juce::dontSendNotification
    //);
    //addAndMakeVisible(env2EnvelopePitchLabel);

    //env2EnvelopePitchSlider.setRange(-50, 50, 1);
    //env2EnvelopePitchSlider.setSliderStyle(
    //    juce::Slider::LinearHorizontal
    //);
    //env2EnvelopePitchSlider.setTextBoxStyle(
    //    juce::Slider::NoTextBox,
    //    false,
    //    0,
    //    0
    //);
    //env2EnvelopePitchSlider.setDoubleClickReturnValue(
    //    true,
    //    0.0
    //);
    //addAndMakeVisible(env2EnvelopePitchSlider);

    //env2EnvelopePitchAmountLabel.setJustificationType(
    //    juce::Justification::centred
    //);
    //addAndMakeVisible(env2EnvelopePitchAmountLabel);

    //env2EnvelopePitchAmountLabel.setText(
    //    "0",
    //    juce::dontSendNotification
    //);


    //env2EnvelopePitchSlider.onValueChange =
    //    [this]()
    //    {
    //        env2EnvelopePitchAmountLabel.setText(
    //            juce::String(
    //                (int)env2EnvelopePitchSlider.getValue()
    //            ),
    //            juce::dontSendNotification
    //        );
    //    };

    //env2EnvelopePitchSlider.onDragEnd =
    //    [this]()
    //    {
    //        const int value =
    //            (int)env2EnvelopePitchSlider.getValue();

    //        if (onEnv2EnvelopePitchChanged)
    //            onEnv2EnvelopePitchChanged(value);
    //    };

    setupModAmountSlider(
        modPitchAmountSlider,
        modPitchAmountLabel
    );

    setupModAmountSlider(
        modPan2AmountSlider,
        modPan2AmountLabel
    );

    setupModAmountSlider(
        modPan3AmountSlider,
        modPan3AmountLabel
    );

    setupModAmountSlider(
        modAmp1AmountSlider,
        modAmp1AmountLabel
    );

    setupModAmountSlider(
        modAmp2AmountSlider,
        modAmp2AmountLabel
    );

    setupModAmountSlider(
        modLfo1RateAmountSlider,
        modLfo1RateAmountLabel
    );

    setupModAmountSlider(
        modLfo1DepthAmountSlider,
        modLfo1DepthAmountLabel
    );

    setupModAmountSlider(
        modLfo1DelayAmountSlider,
        modLfo1DelayAmountLabel
    );

    setupModAmountSlider(
        modFilter1AmountSlider,
        modFilter1AmountLabel
    );

    setupModAmountSlider(
        modFilter2AmountSlider,
        modFilter2AmountLabel
    );
    setupModAmountSlider(
        modFilter3AmountSlider,
        modFilter3AmountLabel
    );

    modFilter1AmountSlider.setDoubleClickReturnValue(
        true,
        0.0
    );

    modFilter2AmountSlider.setDoubleClickReturnValue(
        true,
        0.0
    );

    modFilter3AmountSlider.setDoubleClickReturnValue(
        true,
        0.0
    );

    modPitchAmountSlider.setDoubleClickReturnValue(
        true,
        0.0
    );

    lfo1PitchSlider.setDoubleClickReturnValue(
        true,
        0.0

    );


    modPan1SourceCombo.onChange = [this]()
        {
            currentProgram.modSPan1 =
                modPan1SourceCombo.getSelectedId() - 1;

            DBG("=== BEFORE MOD PAN1 SEND ===");

            DBG("PAN=" + juce::String(currentProgram.pan));
            DBG("LOUDNESS=" + juce::String(currentProgram.loudness));
            DBG("STEREO=" + juce::String(currentProgram.stereoLevel));

            DBG("LFO1 RATE=" + juce::String(currentProgram.lfo1Rate));
            DBG("LFO1 DEPTH=" + juce::String(currentProgram.lfo1Depth));
            DBG("LFO1 DELAY=" + juce::String(currentProgram.lfo1Delay));

            DBG("LFO2 RATE=" + juce::String(currentProgram.lfo2Rate));
            DBG("LFO2 DEPTH=" + juce::String(currentProgram.lfo2Depth));
            DBG("LFO2 DELAY=" + juce::String(currentProgram.lfo2Delay));

            DBG("MOD S PAN1=" + juce::String(currentProgram.modSPan1));
            DBG("MOD S PAN2=" + juce::String(currentProgram.modSPan2));
            DBG("MOD S PAN3=" + juce::String(currentProgram.modSPan3));

            DBG("RAW SIZE=" + juce::String(
                (int)currentProgram.rawData.size()
            ));

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    modPan2SourceCombo.onChange = [this]()
        {
            currentProgram.modSPan2 =
                modPan2SourceCombo.getSelectedId() - 1;

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    modPan3SourceCombo.onChange = [this]()
        {
            currentProgram.modSPan3 =
                modPan3SourceCombo.getSelectedId() - 1;

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };


    modAmp1SourceCombo.onChange = [this]()
        {
            currentProgram.modSAmp1 =
                modAmp1SourceCombo.getSelectedId() - 1;

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    modAmp2SourceCombo.onChange = [this]()
        {
            currentProgram.modSAmp2 =
                modAmp2SourceCombo.getSelectedId() - 1;

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

//    modAmp3SourceCombo.onChange = [this]()
//        {
//            currentProgram.modSAmp3 =
//                modAmp3SourceCombo.getSelectedId() - 1;
//
//            if (onProgramChanged)
//                onProgramChanged(currentProgram);
//        };
    
//    modAmp3SourceCombo.setEnabled(false);


    modLfo1RateSourceCombo.onChange = [this]()
        {
            currentProgram.modSLfo1Rate =
                modLfo1RateSourceCombo.getSelectedId() - 1;

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    modLfo1DepthSourceCombo.onChange = [this]()
        {
            currentProgram.modSLfo1Depth =
                modLfo1DepthSourceCombo.getSelectedId() - 1;

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    modLfo1DelaySourceCombo.onChange = [this]()
        {
            currentProgram.modSLfo1Delay =
                modLfo1DelaySourceCombo.getSelectedId() - 1;

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };


    modFilter1SourceCombo.onChange = [this]()
        {
            const int value =
                modFilter1SourceCombo.getSelectedId() - 1;

            currentProgram.modSFilter1 = value;

            DBG(
                "FILTER1 SOURCE CHANGED VALUE="
                + juce::String(value)
            );

            if (onModFilter1SourceChanged)
                onModFilter1SourceChanged(value);
        };

    modFilter2SourceCombo.onChange = [this]()
        {
            const int value =
                modFilter2SourceCombo.getSelectedId() - 1;

            currentProgram.modSFilter2 = value;

            DBG(
                "FILTER2 SOURCE CHANGED VALUE="
                + juce::String(value)
            );

            if (onModFilter2SourceChanged)
                onModFilter2SourceChanged(value);
        };

    modFilter3SourceCombo.onChange = [this]()
        {
            const int value =
                modFilter3SourceCombo.getSelectedId() - 1;

            currentProgram.modSFilter3 = value;

            DBG(
                "FILTER3 SOURCE CHANGED VALUE="
                + juce::String(value)
            );

            if (onModFilter3SourceChanged)
                onModFilter3SourceChanged(value);
        };

    modPitchSourceCombo.onChange = [this]()
        {
            currentProgram.modSPitch =
                modPitchSourceCombo.getSelectedId() - 1;

            DBG(
                "MOD PITCH SOURCE CHANGED="
                + juce::String(currentProgram.modSPitch)
            );

            if (onModPitchSourceChanged)
                onModPitchSourceChanged(
                    currentProgram.modSPitch
                );
        };



    modPan1AmountSlider.onDragEnd = [this]()
        {
            const int value =
                (int)modPan1AmountSlider.getValue();

            DBG(
                "MOD PAN1 DRAG END"
                " slider="
                + juce::String(value)
                + " model="
                + juce::String(currentProgram.modVPan1)
            );

            // 値が変わっていなければ送信不要
            if (value == currentProgram.modVPan1)
            {
                DBG("MOD PAN1 DRAG END: NO CHANGE -> IGNORE");
                return;
            }

            currentProgram.modVPan1 = value;

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };


    modPan1AmountSlider.onDoubleClickReset = [this]()
        {
            currentProgram.modVPan1 =
                (int)modPan1AmountSlider.getValue();

            DBG(
                "MOD PAN1 DOUBLE CLICK RESET="
                + juce::String(currentProgram.modVPan1)
            );

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };


    modPan2AmountSlider.onDragEnd = [this]()
        {
            currentProgram.modVPan2 =
                (int)modPan2AmountSlider.getValue();

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    modPan3AmountSlider.onDragEnd = [this]()
        {
            currentProgram.modVPan3 =
                (int)modPan3AmountSlider.getValue();

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };


    modAmp1AmountSlider.onDragEnd = [this]()
        {
            currentProgram.modVAmp1 =
                (int)modAmp1AmountSlider.getValue();

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    modAmp2AmountSlider.onDragEnd = [this]()
        {
            currentProgram.modVAmp2 =
                (int)modAmp2AmountSlider.getValue();

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };


    modLfo1RateAmountSlider.onDragEnd = [this]()
        {
            currentProgram.modVLfo1Rate =
                (int)modLfo1RateAmountSlider.getValue();

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    modLfo1DepthAmountSlider.onDragEnd = [this]()
        {
            currentProgram.modVLfo1Depth =
                (int)modLfo1DepthAmountSlider.getValue();

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    modLfo1DelayAmountSlider.onDragEnd = [this]()
        {
            currentProgram.modVLfo1Delay =
                (int)modLfo1DelayAmountSlider.getValue();

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    modFilter1AmountSlider.onValueChange =
        [this]()
        {
            const int value =
                (int)modFilter1AmountSlider.getValue();

            modFilter1AmountLabel.setText(
                value > 0
                ? "+" + juce::String(value)
                : juce::String(value),
                juce::dontSendNotification
            );

            if (onModFilter1Changed)
                onModFilter1Changed(value);
        };

    modFilter2AmountSlider.onDragEnd =
        [this]()
        {
            const int value =
                (int)modFilter2AmountSlider.getValue();

            DBG(
                "FILTER2 AMOUNT EDIT FINISHED="
                + juce::String(value)
            );

            if (onModFilter2Changed)
                onModFilter2Changed(value);
        };

    modFilter3AmountSlider.onDragEnd =
        [this]()
        {
            const int value =
                (int)modFilter3AmountSlider.getValue();

            DBG(
                "FILTER3 AMOUNT EDIT FINISHED="
                + juce::String(value)
            );

            if (onModFilter3Changed)
                onModFilter3Changed(value);
        };

    modPitchAmountSlider.onDragEnd =
        [this]()
        {
            const int value =
                (int)modPitchAmountSlider.getValue();

            DBG(
                "ENV2 PITCH EDIT FINISHED="
                + juce::String(value)
            );

            if (onEnv2PitchChanged)
                onEnv2PitchChanged(value);
        };

    lfo1PitchSlider.onDragEnd =
        [this]()
        {
            const int value =
                (int)lfo1PitchSlider.getValue();

            DBG(
                "LFO1 PITCH EDIT FINISHED="
                + juce::String(value)
            );

            if (onLfo1PitchChanged)
                onLfo1PitchChanged(value);
        };


    lfo1PitchSlider.setDoubleClickReturnValue(
        true,
        0.0
    );


//    velocityLoudnessLabel.setText(
//        "Vel -> Loud",
//        juce::dontSendNotification
//    );
    
    const int value =
        (int)velocityLoudnessSlider.getValue();

//    velocityLoudnessLabel.setText(
//        value > 0
//            ? "+" + juce::String(value)
//            : juce::String(value),
//        juce::dontSendNotification
//    );
    
    
//    addAndMakeVisible(velocityLoudnessLabel);

    velocityLoudnessSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );

    velocityLoudnessSlider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );

    velocityLoudnessSlider.setRange(
        -50.0,
        50.0,
        1.0
    );
    
    velocityLoudnessSlider.setScrollWheelEnabled(false);
    
    loudnessSlider.setScrollWheelEnabled(false);

    addAndMakeVisible(velocityLoudnessSlider);

    velocityLoudnessValueLabel.setJustificationType(
        juce::Justification::centredRight
    );

    addAndMakeVisible(
        velocityLoudnessValueLabel
    );

    velocityLoudnessSlider.onValueChange =
        [this]()
        {
            const int value =
                (int)velocityLoudnessSlider.getValue();

            velocityLoudnessValueLabel.setText(
                value > 0
                    ? "+" + juce::String(value)
                    : juce::String(value),
                juce::dontSendNotification
            );
        };

    velocityLoudnessSlider.onDragEnd =
        [this]()
        {
            currentProgram.velocityLoudness =
                (int)velocityLoudnessSlider.getValue();

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };


    keygroupCrossfadeLabel.setText(
        "KG Crossfade",
        juce::dontSendNotification
    );
    addAndMakeVisible(keygroupCrossfadeLabel);

    keygroupCrossfadeCombo.addItem("OFF", 1);
    keygroupCrossfadeCombo.addItem("ON", 2);
    addAndMakeVisible(keygroupCrossfadeCombo);

    keygroupCrossfadeCombo.onChange = [this]()
        {
            currentProgram.keygroupCrossfade =
                keygroupCrossfadeCombo.getSelectedId() == 2;

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    modWheelDepthLabel.setText(
        "ModWheel -> Depth",
        juce::dontSendNotification
    );
    addAndMakeVisible(modWheelDepthLabel);

    modWheelDepthSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );
    modWheelDepthSlider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );
    addAndMakeVisible(modWheelDepthSlider);

    modWheelDepthValueLabel.setJustificationType(
        juce::Justification::centredRight
    );
    addAndMakeVisible(modWheelDepthValueLabel);

    modWheelDepthSlider.onValueChange = [this]()
        {
            modWheelDepthValueLabel.setText(
                juce::String((int)modWheelDepthSlider.getValue()),
                juce::dontSendNotification
            );
        };

    modWheelDepthSlider.onDragEnd = [this]()
        {
            currentProgram.modWheelDepth =
                (int)modWheelDepthSlider.getValue();

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    pressureDepthLabel.setText(
        "Pressure -> Depth",
        juce::dontSendNotification
    );
    addAndMakeVisible(pressureDepthLabel);

    pressureDepthSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );
    pressureDepthSlider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );
    addAndMakeVisible(pressureDepthSlider);

    pressureDepthValueLabel.setJustificationType(
        juce::Justification::centredRight
    );
    addAndMakeVisible(pressureDepthValueLabel);

    pressureDepthSlider.onValueChange = [this]()
        {
            pressureDepthValueLabel.setText(
                juce::String((int)pressureDepthSlider.getValue()),
                juce::dontSendNotification
            );
        };

    pressureDepthSlider.onDragEnd = [this]()
        {
            currentProgram.pressureDepth =
                (int)pressureDepthSlider.getValue();

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };

    velocityDepthLabel.setText(
        "Velocity -> Depth",
        juce::dontSendNotification
    );
    addAndMakeVisible(velocityDepthLabel);

    velocityDepthSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );
    velocityDepthSlider.setTextBoxStyle(
        juce::Slider::NoTextBox,
        false,
        0,
        0
    );
    addAndMakeVisible(velocityDepthSlider);

    modWheelDepthSlider.setRange(0.0, 99.0, 1.0);
    
    modWheelDepthSlider.setScrollWheelEnabled(false);

    pressureDepthSlider.setRange(0.0, 99.0, 1.0);
    
//    pressureDepthSlider.setScrollWheelEnabled(false);

    velocityDepthSlider.setRange(0.0, 99.0, 1.0);
    
    velocityDepthSlider.setScrollWheelEnabled(false);

    velocityDepthValueLabel.setJustificationType(
        juce::Justification::centredRight
    );
    addAndMakeVisible(velocityDepthValueLabel);

    velocityDepthSlider.onValueChange = [this]()
        {
            velocityDepthValueLabel.setText(
                juce::String((int)velocityDepthSlider.getValue()),
                juce::dontSendNotification
            );
        };

    velocityDepthSlider.onDragEnd = [this]()
        {
            currentProgram.velocityDepth =
                (int)velocityDepthSlider.getValue();

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };
    

    addAndMakeVisible(modulationEditAllToggle);

    modulationEditAllToggle.setToggleState(
        false,
        juce::dontSendNotification
    );

    addAndMakeVisible(portamentoSectionLabel);

    addAndMakeVisible(portamentoEnableToggle);

    addAndMakeVisible(portamentoTypeCombo);
    portamentoTypeCombo.addItem("RATE", 1);
    portamentoTypeCombo.addItem("TIME", 2);

    addAndMakeVisible(portamentoValueSlider);
    portamentoValueSlider.setRange(0, 99, 1);
    portamentoValueSlider.setScrollWheelEnabled(false);
    
    
    portamentoValueSlider.setSliderStyle(
        juce::Slider::LinearHorizontal
    );
    portamentoValueSlider.setTextBoxStyle(
        juce::Slider::TextBoxRight,
        false,
        50,
        20
    );
    
    portamentoValueSlider.setColour(
        juce::Slider::textBoxTextColourId,
        juce::Colour(35, 35, 35)
    );





}

void ProgramEditor::setModFilter1Amount(int value)
{
    modFilter1AmountSlider.setValue(
        value,
        juce::dontSendNotification
    );

    modFilter1AmountLabel.setText(
        juce::String(value),
        juce::dontSendNotification
    );
}

void ProgramEditor::setModFilter2Amount(int value)
{
    modFilter2AmountSlider.setValue(
        value,
        juce::dontSendNotification
    );

    modFilter2AmountLabel.setText(
        juce::String(value),
        juce::dontSendNotification
    );
}

void ProgramEditor::setModFilter3Amount(int value)
{
    modFilter3AmountSlider.setValue(
        value,
        juce::dontSendNotification
    );

    modFilter3AmountLabel.setText(
        value > 0
            ? "+" + juce::String(value)
            : juce::String(value),
        juce::dontSendNotification
    );
}

void ProgramEditor::setModPitchAmount(int value)
{
    modPitchAmountSlider.setValue(
        value,
        juce::dontSendNotification
    );

    modPitchAmountLabel.setText(
        value > 0
        ? "+" + juce::String(value)
        : juce::String(value),
        juce::dontSendNotification
    );
}

void ProgramEditor::setLfo1PitchAmount(int value)
{
    lfo1PitchSlider.setValue(
        value,
        juce::dontSendNotification
    );

    lfo1PitchAmountLabel.setText(
        value > 0
        ? "+" + juce::String(value)
        : juce::String(value),
        juce::dontSendNotification
    );
}


void ProgramEditor::paint(
    juce::Graphics& g)
{
    // S3000XL-style panel background
    g.fillAll(
        juce::Colour::fromRGB(
            198, 197, 191
        )
    );

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
    drawCard(lfo2CardBounds);
    drawCard(softPedalCardBounds);

    drawCard(portamentoCardBounds);

    drawCard(modPanCardBounds);
    drawCard(modAmpCardBounds);
    drawCard(modLfo1CardBounds);
    drawCard(modFilterPitchCardBounds);
}


void ProgramEditor::setProgram(
    const Program& program)
{
    DBG("ProgramEditor SETPROGRAM this="
        + juce::String::toHexString(
            reinterpret_cast<juce::pointer_sized_int>(this)));

    DBG("=== ProgramEditor::setProgram ENTER ===");
    DBG("PROGRAM=" + juce::String(program.programNumber)
        + " LFO1 RATE=" + juce::String(program.lfo1Rate)
        + " PAN=" + juce::String(program.pan));

    currentProgram = program;

    programNameEditor.setText(
        juce::String(program.name),
        juce::dontSendNotification
    );

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

    DBG("AFTER SET PAN = "
        + juce::String(panSlider.getValue()));

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

    DBG("AFTER SET LFO1 RATE = "
        + juce::String(lfo1RateSlider.getValue()));

    lfo1RateValueLabel.setText(
        juce::String(program.lfo1Rate),
        juce::dontSendNotification

    );

    lfo1DepthSlider.setValue(
        program.lfo1Depth,
        juce::dontSendNotification
    );

    lfo1DepthValueLabel.setText(
        juce::String(program.lfo1Depth),
        juce::dontSendNotification
    );

    lfo1DelaySlider.setValue(
        program.lfo1Delay,
        juce::dontSendNotification
    );

    lfo1DelayValueLabel.setText(
        juce::String(program.lfo1Delay),
        juce::dontSendNotification
    );

    lfo1WaveLabel.setText(
        "Wave",
        juce::dontSendNotification
    );

    addAndMakeVisible(lfo1WaveLabel);



    lfo1WaveCombo.setSelectedId(
        program.lfo1Wave + 1,
        juce::dontSendNotification
    );

    lfo2RateSlider.setValue(
        program.lfo2Rate,
        juce::dontSendNotification
    );

    lfo2RateValueLabel.setText(
        juce::String(program.lfo2Rate),
        juce::dontSendNotification
    );

    lfo2DepthSlider.setValue(
        program.lfo2Depth,
        juce::dontSendNotification
    );

    lfo2DepthValueLabel.setText(
        juce::String(program.lfo2Depth),
        juce::dontSendNotification
    );

    lfo2DelaySlider.setValue(
        program.lfo2Delay,
        juce::dontSendNotification
    );

    lfo2DelayValueLabel.setText(
        juce::String(program.lfo2Delay),
        juce::dontSendNotification
    );

    lfo2WaveCombo.setSelectedId(
        program.lfo2Wave + 1,
        juce::dontSendNotification
    );

    velocityLoudnessSlider.setValue(
        program.velocityLoudness,
        juce::dontSendNotification
    );

    velocityLoudnessValueLabel.setText(
        program.velocityLoudness > 0
            ? "+" + juce::String(program.velocityLoudness)
            : juce::String(program.velocityLoudness),
        juce::dontSendNotification
    );



    softLoudnessSlider.setValue(
        program.softLoudness,
        juce::dontSendNotification
    );

    softLoudnessValueLabel.setText(
        juce::String(program.softLoudness),
        juce::dontSendNotification
    );

    softAttackSlider.setValue(
        program.softAttack,
        juce::dontSendNotification
    );

    softAttackValueLabel.setText(
        juce::String(program.softAttack),
        juce::dontSendNotification
    );

    softFilterSlider.setValue(
        program.softFilter,
        juce::dontSendNotification
    );

    softFilterValueLabel.setText(
        juce::String(program.softFilter),
        juce::dontSendNotification
    );

    // =========================
// MODULATION - SOURCES
// =========================

    modPan1SourceCombo.setSelectedId(
        program.modSPan1 + 1,
        juce::dontSendNotification
    );



    modPan2SourceCombo.setSelectedId(
        program.modSPan2 + 1,
        juce::dontSendNotification
    );

    modPan3SourceCombo.setSelectedId(
        program.modSPan3 + 1,
        juce::dontSendNotification
    );


    modAmp1SourceCombo.setSelectedId(
        program.modSAmp1 + 1,
        juce::dontSendNotification
    );

    modAmp2SourceCombo.setSelectedId(
        program.modSAmp2 + 1,
        juce::dontSendNotification
    );

//    modAmp3SourceCombo.setSelectedId(
//        program.modSAmp3 + 1,
//        juce::dontSendNotification
//    );
    
    modAmp3SourceLabel.setText(
        "Velocity",
        juce::dontSendNotification
    );
    
    addAndMakeVisible(modAmp3Label);
    
    modAmp3Label.setText(
        "Source 3",
        juce::dontSendNotification
    );
    
    

    addAndMakeVisible(modAmp3SourceLabel);


    modLfo1RateSourceCombo.setSelectedId(
        program.modSLfo1Rate + 1,
        juce::dontSendNotification
    );

    modLfo1DepthSourceCombo.setSelectedId(
        program.modSLfo1Depth + 1,
        juce::dontSendNotification
    );

    modLfo1DelaySourceCombo.setSelectedId(
        program.modSLfo1Delay + 1,
        juce::dontSendNotification
    );


    modFilter1SourceCombo.setSelectedId(
        program.modSFilter1 + 1,
        juce::dontSendNotification
    );

    modFilter2SourceCombo.setSelectedId(
        program.modSFilter2 + 1,
        juce::dontSendNotification
    );

    modFilter3SourceCombo.setSelectedId(
        program.modSFilter3 + 1,
        juce::dontSendNotification
    );

    modPitchSourceCombo.setSelectedId(
        program.modSPitch + 1,
        juce::dontSendNotification
    );


    // =========================
    // MODULATION - AMOUNTS
    // =========================

    modPan1AmountSlider.setValue(
        program.modVPan1,
        juce::dontSendNotification
    );

    modPan2AmountSlider.setValue(
        program.modVPan2,
        juce::dontSendNotification
    );

    modPan3AmountSlider.setValue(
        program.modVPan3,
        juce::dontSendNotification
    );

    modPan1AmountSlider.setDoubleClickReturnValue(true, 0.0);
    modPan2AmountSlider.setDoubleClickReturnValue(true, 0.0);
    modPan3AmountSlider.setDoubleClickReturnValue(true, 0.0);


    modAmp1AmountSlider.setValue(
        program.modVAmp1,
        juce::dontSendNotification
    );

    modAmp2AmountSlider.setValue(
        program.modVAmp2,
        juce::dontSendNotification
    );
    modAmp1AmountSlider.setDoubleClickReturnValue(
        true,
        0.0
    );

    modAmp2AmountSlider.setDoubleClickReturnValue(
        true,
        0.0
    );



    modLfo1RateAmountSlider.setValue(
        program.modVLfo1Rate,
        juce::dontSendNotification
    );

    modLfo1DepthAmountSlider.setValue(
        program.modVLfo1Depth,
        juce::dontSendNotification
    );

    modLfo1DelayAmountSlider.setValue(
        program.modVLfo1Delay,
        juce::dontSendNotification
    );

    modLfo1RateAmountSlider.setDoubleClickReturnValue(
        true,
        0.0
    );

    modLfo1DepthAmountSlider.setDoubleClickReturnValue(
        true,
        0.0
    );

    modLfo1DelayAmountSlider.setDoubleClickReturnValue(
        true,
        0.0
    );

    auto setAmountLabel =
        [](juce::Label& label, int value)
        {
            label.setText(
                value > 0
                ? "+" + juce::String(value)
                : juce::String(value),
                juce::dontSendNotification
            );
        };

    setAmountLabel(
        modPan1AmountLabel,
        program.modVPan1
    );

    setAmountLabel(
        modPan2AmountLabel,
        program.modVPan2
    );

    setAmountLabel(
        modPan3AmountLabel,
        program.modVPan3
    );

    setAmountLabel(
        modAmp1AmountLabel,
        program.modVAmp1
    );

    setAmountLabel(
        modAmp2AmountLabel,
        program.modVAmp2
    );

    setAmountLabel(
        modLfo1RateAmountLabel,
        program.modVLfo1Rate
    );

    setAmountLabel(
        modLfo1DepthAmountLabel,
        program.modVLfo1Depth
    );

    setAmountLabel(
        modLfo1DelayAmountLabel,
        program.modVLfo1Delay
    );

    //repaint();

    /*keygroupCrossfadeLabel.setText(
        "KG Crossfade",
        juce::dontSendNotification
    );
    addAndMakeVisible(keygroupCrossfadeLabel);

    keygroupCrossfadeCombo.addItem("OFF", 1);
    keygroupCrossfadeCombo.addItem("ON", 2);
    addAndMakeVisible(keygroupCrossfadeCombo);

    keygroupCrossfadeCombo.onChange = [this]()
        {
            currentProgram.keygroupCrossfade =
                keygroupCrossfadeCombo.getSelectedId() == 2;

            if (onProgramChanged)
                onProgramChanged(currentProgram);
        };
*/
    keygroupCrossfadeCombo.setSelectedId(
        program.keygroupCrossfade ? 2 : 1,
        juce::dontSendNotification
    );

    modWheelDepthSlider.setValue(
        program.modWheelDepth,
        juce::dontSendNotification
    );

    modWheelDepthValueLabel.setText(
        juce::String(program.modWheelDepth),
        juce::dontSendNotification
    );

    pressureDepthSlider.setValue(
        program.pressureDepth,
        juce::dontSendNotification
    );

    pressureDepthValueLabel.setText(
        juce::String(program.pressureDepth),
        juce::dontSendNotification
    );

    velocityDepthSlider.setValue(
        program.velocityDepth,
        juce::dontSendNotification
    );

    velocityDepthValueLabel.setText(
        juce::String(program.velocityDepth),
        juce::dontSendNotification
    );

    /*modPitchAmountSlider.setValue(
        program.modVPitch,
        juce::dontSendNotification
    );*/

    modPitchAmountLabel.setText(
        program.modVPitch > 0
        ? "+" + juce::String(program.modVPitch)
        : juce::String(program.modVPitch),
        juce::dontSendNotification
    );

    portamentoEnableToggle.setToggleState(
        program.portamentoEnabled,
        juce::dontSendNotification
    );

    portamentoTypeCombo.setSelectedId(
        program.portamentoType == 0 ? 1 : 2,
        juce::dontSendNotification
    );

    portamentoValueSlider.setValue(
        program.portamentoTime,
        juce::dontSendNotification
    );
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
        left.removeFromTop(326);

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

        programNameLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        programNameEditor.setBounds(
            row.reduced(2)
        );
    }

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

//    {
//        auto row =
//            general.removeFromTop(rowHeight);
//
////        velocityLoudnessLabel.setBounds(
////            row.removeFromLeft(labelWidth)
////        );
//
//        velocityLoudnessValueLabel.setBounds(
//            row.removeFromRight(55)
//        );
//
//        velocityLoudnessSlider.setBounds(
//            row.reduced(2)
//        );
//    }

    {
        auto row =
            general.removeFromTop(rowHeight);

        keygroupCrossfadeLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        keygroupCrossfadeCombo.setBounds(
            row.reduced(2)
        );
    }



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
// SOFT PEDAL
// =========================

    /*left.removeFromTop(10);

    softPedalCardBounds =
        left.removeFromTop(130);

    auto softPedal =
        softPedalCardBounds.reduced(10);

    softPedalSectionLabel.setBounds(
        softPedal.removeFromTop(24)
    );

    {
        auto row =
            softPedal.removeFromTop(rowHeight);

        softLoudnessLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        softLoudnessValueLabel.setBounds(
            row.removeFromRight(55)
        );

        softLoudnessSlider.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row =
            softPedal.removeFromTop(rowHeight);

        softAttackLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        softAttackValueLabel.setBounds(
            row.removeFromRight(55)
        );

        softAttackSlider.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row =
            softPedal.removeFromTop(rowHeight);

        softFilterLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        softFilterValueLabel.setBounds(
            row.removeFromRight(55)
        );

        softFilterSlider.setBounds(
            row.reduced(2)
        );
    }

    left.removeFromTop(10);*/

    // =========================
// PORTAMENTO
// =========================

    left.removeFromTop(10);

    portamentoCardBounds =
        left.removeFromTop(120);

    auto portamento =
        portamentoCardBounds.reduced(10);

    portamentoSectionLabel.setBounds(
        portamento.removeFromTop(24)
    );

    // ON / OFF
    {
        auto row =
            portamento.removeFromTop(rowHeight);

        portamentoEnableToggle.setBounds(
            row.removeFromLeft(100)
        );
    }

    // TYPE + VALUE
    {
        auto row =
            portamento.removeFromTop(rowHeight);

        portamentoTypeCombo.setBounds(
            row.removeFromLeft(120).reduced(2)
        );

        portamentoValueSlider.setBounds(
            row.reduced(2)
        );
    }



    left.removeFromTop(10);

    // =========================
    // MODULATION - PAN
    // =========================

    modPanCardBounds = left.removeFromTop(160);

    auto modPan = modPanCardBounds.reduced(10);

    modPanSectionLabel.setBounds(
        modPan.removeFromTop(24)
    );

    const int modRowHeight = 38;
    const int modNameWidth = 70;
    const int modSourceWidth = 135;
    const int modValueWidth = 45;

    auto layoutModAmountRow =
        [&](juce::Rectangle<int>& section,
            juce::Label& nameLabel,
            juce::ComboBox& sourceCombo,
            juce::Slider& amountSlider,
            juce::Label& amountLabel)
        {
            auto row =
                section.removeFromTop(modRowHeight);

            nameLabel.setBounds(
                row.removeFromLeft(modNameWidth)
            );

            sourceCombo.setBounds(
                row.removeFromLeft(modSourceWidth)
                .reduced(2, 4)
            );

            amountLabel.setBounds(
                row.removeFromRight(modValueWidth)
            );

            amountSlider.setBounds(
                row.reduced(4, 7)
            );
        };

    layoutModAmountRow(
        modPan,
        modPan1Label,
        modPan1SourceCombo,
        modPan1AmountSlider,
        modPan1AmountLabel
    );

    layoutModAmountRow(
        modPan,
        modPan2Label,
        modPan2SourceCombo,
        modPan2AmountSlider,
        modPan2AmountLabel
    );

    layoutModAmountRow(
        modPan,
        modPan3Label,
        modPan3SourceCombo,
        modPan3AmountSlider,
        modPan3AmountLabel
    );

    left.removeFromTop(10);

    // =========================
    // MODULATION - LOUDNESS
    // =========================

    modAmpCardBounds = left.removeFromTop(160);

    auto modAmp = modAmpCardBounds.reduced(10);

    modAmpSectionLabel.setBounds(
        modAmp.removeFromTop(24)
    );

    layoutModAmountRow(
        modAmp,
        modAmp1Label,
        modAmp1SourceCombo,
        modAmp1AmountSlider,
        modAmp1AmountLabel
    );

    layoutModAmountRow(
        modAmp,
        modAmp2Label,
        modAmp2SourceCombo,
        modAmp2AmountSlider,
        modAmp2AmountLabel
    );

    // Source 3 = Velocity -> Loudness
    {
        auto row =
            modAmp.removeFromTop(modRowHeight);

        modAmp3Label.setBounds(
            row.removeFromLeft(modNameWidth)
        );

        modAmp3SourceLabel.setBounds(
            row.removeFromLeft(modSourceWidth)
            .reduced(2, 4)
        );

        velocityLoudnessValueLabel.setBounds(
            row.removeFromRight(modValueWidth)
        );

        velocityLoudnessSlider.setBounds(
            row.reduced(4, 7)
        );
    }

    // =========================
    // LFO1
    // =========================

    lfo1CardBounds =
        right.removeFromTop(272);

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

    {
        auto row =
            lfo1.removeFromTop(rowHeight);

        lfo1DepthLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        lfo1DepthValueLabel.setBounds(
            row.removeFromRight(55)
        );

        lfo1DepthSlider.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row =
            lfo1.removeFromTop(rowHeight);

        lfo1DelayLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        lfo1DelayValueLabel.setBounds(
            row.removeFromRight(55)
        );

        lfo1DelaySlider.setBounds(
            row.reduced(2)
        );
    }


    {
        auto row =
            lfo1.removeFromTop(rowHeight);

        lfo1WaveLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        lfo1WaveCombo.setBounds(
            row.reduced(2)
        );
    }
    {
        auto row = lfo1.removeFromTop(rowHeight);

        modWheelDepthLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        modWheelDepthValueLabel.setBounds(
            row.removeFromRight(55)
        );

        modWheelDepthSlider.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row = lfo1.removeFromTop(rowHeight);

        pressureDepthLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        pressureDepthValueLabel.setBounds(
            row.removeFromRight(55)
        );

        pressureDepthSlider.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row = lfo1.removeFromTop(rowHeight);

        velocityDepthLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        velocityDepthValueLabel.setBounds(
            row.removeFromRight(55)
        );

        velocityDepthSlider.setBounds(
            row.reduced(2)
        );
    }



    right.removeFromTop(10);

    lfo2CardBounds =
        right.removeFromTop(158);

    auto lfo2 =
        lfo2CardBounds.reduced(10);

    lfo2SectionLabel.setBounds(
        lfo2.removeFromTop(24)
    );

    {
        auto row = lfo2.removeFromTop(rowHeight);

        lfo2RateLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        lfo2RateValueLabel.setBounds(
            row.removeFromRight(55)
        );

        lfo2RateSlider.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row = lfo2.removeFromTop(rowHeight);

        lfo2DepthLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        lfo2DepthValueLabel.setBounds(
            row.removeFromRight(55)
        );

        lfo2DepthSlider.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row = lfo2.removeFromTop(rowHeight);

        lfo2DelayLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        lfo2DelayValueLabel.setBounds(
            row.removeFromRight(55)
        );

        lfo2DelaySlider.setBounds(
            row.reduced(2)
        );
    }

    {
        auto row = lfo2.removeFromTop(rowHeight);

        lfo2WaveLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        lfo2WaveCombo.setBounds(
            row.reduced(2)
        );
    }

    right.removeFromTop(10);

    // =========================
    // MODULATION - LFO1
    // =========================

    modLfo1CardBounds = right.removeFromTop(160);

    auto modLfo1 = modLfo1CardBounds.reduced(10);

    modLfo1SectionLabel.setBounds(
        modLfo1.removeFromTop(24)
    );

    layoutModAmountRow(
        modLfo1,
        modLfo1RateLabel,
        modLfo1RateSourceCombo,
        modLfo1RateAmountSlider,
        modLfo1RateAmountLabel
    );

    layoutModAmountRow(
        modLfo1,
        modLfo1DepthLabel,
        modLfo1DepthSourceCombo,
        modLfo1DepthAmountSlider,
        modLfo1DepthAmountLabel
    );

    layoutModAmountRow(
        modLfo1,
        modLfo1DelayLabel,
        modLfo1DelaySourceCombo,
        modLfo1DelayAmountSlider,
        modLfo1DelayAmountLabel
    );

    right.removeFromTop(10);

    // =========================
    // MODULATION - FILTER / PITCH
    // =========================

    modFilterPitchCardBounds =
        right.removeFromTop(230);

    auto modFilterPitch =
        modFilterPitchCardBounds.reduced(10);

    {
        auto header =
            modFilterPitch.removeFromTop(24);

        modulationEditAllToggle.setBounds(
            header.removeFromRight(70)
        );

        modFilterPitchSectionLabel.setBounds(
            header
        );
    }

    auto layoutModSourceRow =
        [&](juce::Rectangle<int>& section,
            juce::Label& nameLabel,
            juce::ComboBox& sourceCombo)
        {
            auto row =
                section.removeFromTop(modRowHeight);

            nameLabel.setBounds(
                row.removeFromLeft(modNameWidth)
            );

            sourceCombo.setBounds(
                row.reduced(2, 4)
            );
        };

    layoutModAmountRow(
        modFilterPitch,
        modFilter1Label,
        modFilter1SourceCombo,
        modFilter1AmountSlider,
        modFilter1AmountLabel
    );

    layoutModAmountRow(
        modFilterPitch,
        modFilter2Label,
        modFilter2SourceCombo,
        modFilter2AmountSlider,
        modFilter2AmountLabel
    );

    layoutModAmountRow(
        modFilterPitch,
        modFilter3Label,
        modFilter3SourceCombo,
        modFilter3AmountSlider,
        modFilter3AmountLabel
    );

    layoutModAmountRow(
        modFilterPitch,
        modPitchLabel,
        modPitchSourceCombo,
        modPitchAmountSlider,
        modPitchAmountLabel
    );

    {
        auto row =
            modFilterPitch.removeFromTop(modRowHeight);

        lfo1PitchLabel.setBounds(
            row.removeFromLeft(modNameWidth)
        );

        lfo1PitchAmountLabel.setBounds(
            row.removeFromRight(modValueWidth)
        );

        lfo1PitchSlider.setBounds(
            row.reduced(4, 7)
        );
    }




}
