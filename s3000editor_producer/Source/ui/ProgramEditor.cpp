#include "ProgramEditor.h"

ProgramEditor::ProgramEditor()
{
    titleLabel.setText(
        "Program Header",
        juce::dontSendNotification
    );

    addAndMakeVisible(titleLabel);

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

    setupRow(
        midiChannelLabel,
        midiChannelEditor,
        "MIDI Channel"
    );

    setupRow(
        polyphonyLabel,
        polyphonyEditor,
        "Polyphony"
    );

    setupRow(
        priorityLabel,
        priorityEditor,
        "Priority"
    );

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

    setupRow(
        outputLabel,
        outputEditor,
        "Output"
    );

    setupRow(
        stereoLabel,
        stereoEditor,
        "Stereo Level"
    );

    setupRow(
        panLabel,
        panEditor,
        "Pan"
    );

    setupRow(
        loudnessLabel,
        loudnessEditor,
        "Loudness"
    );

    setupRow(
        tuneLabel,
        tuneEditor,
        "Program Tune"
    );
}
void ProgramEditor::setProgram(
    const Program& program)
{
    currentProgram = program;

    // raw 0 = MIDI CH 1
    if (program.midiChannel == 255)
    {
        midiChannelEditor.setText(
            "OMNI",
            false
        );
    }
    else
    {
        midiChannelEditor.setText(
            juce::String(
                program.midiChannel + 1
            ),
            false
        );
    }

    // raw 0-31 -> actual 1-32
    polyphonyEditor.setText(
        juce::String(
            program.polyphony + 1
        ),
        false
    );

    priorityEditor.setText(
        juce::String(program.priority),
        false
    );

    playLowEditor.setText(
        juce::String(program.playLow),
        false
    );

    playHighEditor.setText(
        juce::String(program.playHigh),
        false
    );

    outputEditor.setText(
        program.output == 255
        ? "OFF"
        : juce::String(program.output + 1),
        false
    );

    stereoEditor.setText(
        juce::String(program.stereoLevel),
        false
    );

    panEditor.setText(
        juce::String(program.pan),
        false
    );

    loudnessEditor.setText(
        juce::String(program.loudness),
        false
    );

    tuneEditor.setText(
        juce::String(program.tune, 2),
        false
    );

    DBG("PROGRAM EDITOR UPDATED");
}

void ProgramEditor::resized()
{
    auto area =
        getLocalBounds().reduced(10);

    titleLabel.setBounds(
        area.removeFromTop(30)
    );

    constexpr int rowHeight = 26;
    constexpr int labelWidth = 140;

    auto addRow =
        [&](
            juce::Label& label,
            juce::TextEditor& editor
            )
        {
            auto row =
                area.removeFromTop(rowHeight);

            label.setBounds(
                row.removeFromLeft(labelWidth)
            );

            editor.setBounds(row);
        };

    addRow(
        midiChannelLabel,
        midiChannelEditor
    );

    addRow(
        polyphonyLabel,
        polyphonyEditor
    );

    addRow(
        priorityLabel,
        priorityEditor
    );

    addRow(
        playLowLabel,
        playLowEditor
    );

    addRow(
        playHighLabel,
        playHighEditor
    );

    addRow(
        outputLabel,
        outputEditor
    );

    addRow(
        stereoLabel,
        stereoEditor
    );

    addRow(
        panLabel,
        panEditor
    );

    addRow(
        loudnessLabel,
        loudnessEditor
    );

    addRow(
        tuneLabel,
        tuneEditor
    );
}
