#include "SampleHeaderEditor.h"

SampleHeaderEditor::SampleHeaderEditor()


{
    titleLabel.setText(
        "Sample Header",
        juce::dontSendNotification
    );

    titleLabel.setFont(
        juce::Font(18.0f, juce::Font::bold)
    );

    //loop1PositionLabel.setText(
    //    "Loop 1 Position",
    //    juce::dontSendNotification
    //);

    //addAndMakeVisible(loop1PositionLabel);
    //addAndMakeVisible(loop1PositionEditor);


    addAndMakeVisible(titleLabel);

    nameEditor.onFocusLost = [this]()
        {
            if (!hasValidHeader)
            {
                DBG("IGNORED SAMPLE EDIT: NO VALID HEADER");
                return;
            }

            const auto newName =
                nameEditor.getText();

            if (newName == currentHeader.name)
                return;

            currentHeader.name = newName;

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);
        };

    idEditor.onFocusLost = [this]()
        {
            currentHeader.id =
                idEditor.getText().getIntValue();

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);
        };

    //bandwidthEditor.onFocusLost = [this]()
    //    {
    //        const int value =
    //            bandwidthEditor.getText().getIntValue();

    //        // Bandwidth ÇÕ 0 / 1 ÇÃÇ›
    //        if (value < 0 || value > 1)
    //        {
    //            bandwidthEditor.setText(
    //                juce::String(currentHeader.bandwidth),
    //                false
    //            );

    //            DBG("INVALID BANDWIDTH = " + juce::String(value));
    //            return;
    //        }

    //        if (value == currentHeader.bandwidth)
    //            return;

    //        currentHeader.bandwidth = value;

    //        DBG(
    //            "BANDWIDTH UI CHANGED = "
    //            + juce::String(value)
    //        );

    //        if (onSampleHeaderChanged)
    //            onSampleHeaderChanged(currentHeader);
    //    };

    bandwidthCombo.onChange = [this]()
        {
            const int selectedId =
                bandwidthCombo.getSelectedId();

            if (selectedId <= 0)
                return;

            // ComboBox:
            // 1 -> 10 kHz -> raw 0
            // 2 -> 20 kHz -> raw 1
            const int value =
                selectedId - 1;

            if (value == currentHeader.bandwidth)
                return;

            currentHeader.bandwidth = value;

            DBG(
                "BANDWIDTH COMBO CHANGED = "
                + juce::String(value)
            );

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);
        };

    pitchEditor.onFocusLost = [this]()
        {
            const int newPitch =
                pitchEditor.getText().getIntValue();

            if (newPitch < 0 || newPitch > 127)
            {
                pitchEditor.setText(
                    juce::String(currentHeader.originalPitch),
                    false
                );

                pitchNoteLabel.setText(
                    midiNoteName(currentHeader.originalPitch),
                    juce::dontSendNotification
                );

                return;
            }

            currentHeader.originalPitch = newPitch;

            pitchNoteLabel.setText(
                midiNoteName(currentHeader.originalPitch),
                juce::dontSendNotification
            );

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);
        };

    lengthEditor.onFocusLost = [this]()
        {
            currentHeader.length =
                (uint32_t)lengthEditor.getText().getLargeIntValue();

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);
        };

    activeLoopCombo.addItem("0", 1);
    activeLoopCombo.addItem("1", 2);
    activeLoopCombo.addItem("2", 3);
    activeLoopCombo.addItem("3", 4);

    //activeLoopCombo.onChange = [this]()
    //    {
    //        currentHeader.activeLoop =
    //            activeLoopCombo.getSelectedId() - 1;

    //        if (onSampleHeaderChanged)
    //            onSampleHeaderChanged(currentHeader);
    //    };


    // ==============================
    // Number of Loops ComboBox
    // ==============================

    numLoopsCombo.addItem("0", 1);
    numLoopsCombo.addItem("1", 2);
    numLoopsCombo.addItem("2", 3);
    numLoopsCombo.addItem("3", 4);
    numLoopsCombo.addItem("4", 5);

    numLoopsCombo.onChange = [this]()
        {
            currentHeader.numLoops =
                numLoopsCombo.getSelectedId() - 1;

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);
        };


    playTypeCombo.addItem("Normal looping", 1);
    playTypeCombo.addItem("Loop until release", 2);
    playTypeCombo.addItem("No looping", 3);
    playTypeCombo.addItem("Play to sample end", 4);

    playTypeCombo.onChange = [this]()
        {
            // ComboBox ID ÇÕ1énÇ‹ÇËÇ»ÇÃÇ≈ -1
            currentHeader.playType =
                playTypeCombo.getSelectedId() - 1;

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);
        };

    tuneEditor.onFocusLost = [this]()
        {
            const double newTune =
                tuneEditor.getText().getDoubleValue();

            if (newTune < -50.0 || newTune > 50.0)
            {
                tuneEditor.setText(
                    juce::String(currentHeader.tune, 2),
                    false
                );

                return;
            }

            currentHeader.tune = newTune;

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);
        };

    holdLoopTuneEditor.onFocusLost = [this]()
        {
            const int newValue =
                holdLoopTuneEditor.getText().getIntValue();

            if (newValue < -50 || newValue > 50)
            {
                holdLoopTuneEditor.setText(
                    juce::String((int)currentHeader.holdLoopTune),
                    false
                );

                return;
            }

            currentHeader.holdLoopTune =
                static_cast<int8_t>(newValue);

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);
        };

    startEditor.onFocusLost = [this]()
        {
            const auto newStart =
                static_cast<uint32_t>(
                    startEditor.getText().getLargeIntValue()
                    );

            if (newStart > currentHeader.end)
            {
                startEditor.setText(
                    juce::String(currentHeader.start),
                    false
                );
                return;
            }

            currentHeader.start = newStart;

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);
        };


    endEditor.onFocusLost = [this]()
        {
            const auto newEnd =
                static_cast<uint32_t>(
                    endEditor.getText().getLargeIntValue()
                    );

            if (newEnd < currentHeader.start ||
                newEnd > currentHeader.length)
            {
                endEditor.setText(
                    juce::String(currentHeader.end),
                    false
                );
                return;
            }

            currentHeader.end = newEnd;

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);
        };

    //loop1PositionEditor.onFocusLost = [this]()
    //    {
    //        currentHeader.loops[0].position =
    //            static_cast<uint32_t>(
    //                loop1PositionEditor.getText().getLargeIntValue()
    //                );

    //        if (onSampleHeaderChanged)
    //            onSampleHeaderChanged(currentHeader);
    //    };

    loopPositionEditor.onFocusLost = [this]()
        {
            const auto newPosition =
                static_cast<uint32_t>(
                    loopPositionEditor
                    .getText()
                    .getLargeIntValue()
                    );

            const auto& loop =
                currentHeader.loops[selectedLoopIndex];

            if (newPosition > currentHeader.length)
            {
                loopPositionEditor.setText(
                    juce::String((juce::int64)loop.position),
                    false
                );
                return;
            }

            currentHeader.loops[selectedLoopIndex].position =
                newPosition;

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);
        };

    loopLengthEditor.onFocusLost = [this]()
        {
            const double newLength =
                loopLengthEditor
                .getText()
                .getDoubleValue();

            const auto& loop =
                currentHeader.loops[selectedLoopIndex];

            if (newLength < 0.0 ||
                static_cast<double>(loop.position) + newLength >
                static_cast<double>(currentHeader.length))
            {
                loopLengthEditor.setText(
                    juce::String(loop.length, 3),
                    false
                );
                return;
            }

            currentHeader.loops[selectedLoopIndex].length =
                newLength;

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);
        };

    sampleRateEditor.onFocusLost = [this]()
        {
            const int value =
                sampleRateEditor.getText().getIntValue();

            // 16-bit fieldÇ»ÇÃÇ≈ç≈í·å¿Ç±ÇÃîÕàÕ
            if (value < 0 || value > 65535)
            {
                sampleRateEditor.setText(
                    juce::String(currentHeader.sampleRate),
                    false
                );
                return;
            }

            if (value == currentHeader.sampleRate)
                return;

            currentHeader.sampleRate = value;

            DBG(
                "SAMPLE RATE CHANGED = "
                + juce::String(value)
            );

            if (onSampleHeaderChanged)
            {
                onSampleHeaderChanged(
                    currentHeader
                );
            }
        };


    loopDwellEditor.onFocusLost = [this]()
        {
            const int newDwell =
                loopDwellEditor
                .getText()
                .getIntValue();

            const auto oldDwell =
                currentHeader
                .loops[selectedLoopIndex]
                .dwell;

            if (newDwell < 0 || newDwell > 9999)
            {
                loopDwellEditor.setText(
                    juce::String(oldDwell),
                    false
                );

                return;
            }

            currentHeader
                .loops[selectedLoopIndex]
                .dwell =
                static_cast<uint16_t>(newDwell);

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);

            updateLoopDwellStatus();
        };

    //lengthEditor.onFocusLost = [this]()
    //    {
    //        currentHeader.length =
    //            static_cast<uint64_t>(
    //                lengthEditor.getText().getLargeIntValue()
    //                );

    //        if (onSampleHeaderChanged)
    //            onSampleHeaderChanged(currentHeader);
    //    };

    pitchNoteLabel.setText(
        "",
        juce::dontSendNotification
    );

    addAndMakeVisible(pitchNoteLabel);

    // Sample Rate Valid
    sampleRateValidCombo.addItem("Invalid", 1);
    sampleRateValidCombo.addItem("Valid", 2);

    sampleRateValidCombo.setEnabled(false);

    addAndMakeVisible(sampleRateValidLabel);
    addAndMakeVisible(sampleRateValidCombo);





    // =========================================================
    // Labels
    // =========================================================

    idLabel.setText(
        "ID",
        juce::dontSendNotification
    );

    nameLabel.setText(
        "Name",
        juce::dontSendNotification
    );

    bandwidthLabel.setText(
        "Bandwidth",
        juce::dontSendNotification
    );

    addAndMakeVisible(bandwidthLabel);
    addAndMakeVisible(bandwidthCombo);

    bandwidthCombo.addItem("10 kHz", 1);
    bandwidthCombo.addItem("20 kHz", 2);

    bandwidthCombo.onChange = [this]()
        {
            const int selectedId =
                bandwidthCombo.getSelectedId();

            if (selectedId <= 0)
                return;

            // ComboBox ID 1 Å® S3000XL value 0
            // ComboBox ID 2 Å® S3000XL value 1
            const int bandwidth = selectedId - 1;

            if (bandwidth == currentHeader.bandwidth)
                return;

            currentHeader.bandwidth = bandwidth;

            DBG(
                "BANDWIDTH COMBO CHANGED = "
                + juce::String(currentHeader.bandwidth)
            );

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);
        };

    pitchLabel.setText(
        "Original Pitch",
        juce::dontSendNotification
    );

    sampleRateValidLabel.setText(
        "Sample Rate Valid",
        juce::dontSendNotification
    );

    numLoopsLabel.setText(
        "Number of Loops",
        juce::dontSendNotification
    );

    activeLoopLabel.setText(
        "Active Loop",
        juce::dontSendNotification
    );

    highestLoopLabel.setText(
        "Highest Loop",
        juce::dontSendNotification
    );

    //playTypeLabel.setText(
    //    "Play Type",
    //    juce::dontSendNotification
    //);

    tuneLabel.setText(
        "Tune",
        juce::dontSendNotification
    );

    locationLabel.setText(
        "Location",
        juce::dontSendNotification
    );

    lengthLabel.setText(
        "Length",
        juce::dontSendNotification
    );

    startLabel.setText(
        "Start",
        juce::dontSendNotification
    );

    endLabel.setText(
        "End",
        juce::dontSendNotification
    );

    //loopsLabel.setText(
    //    "Loops",
    //    juce::dontSendNotification
    //);

    sampleRateLabel.setText(
        "Sample Rate",
        juce::dontSendNotification
    );

    holdLoopTuneLabel.setText(
        "Hold Loop Tune",
        juce::dontSendNotification
    );

    //loop1LengthLabel.setText(
    //    "Loop 1 Length",
    //    juce::dontSendNotification
    //);

    //loop1DwellLabel.setText(
    //    "Loop 1 Dwell",
    //    juce::dontSendNotification
    //);

    //addAndMakeVisible(loop1DwellLabel);
    //addAndMakeVisible(loop1DwellEditor);

    //loop1DwellEditor.onFocusLost = [this]()
    //    {
    //        currentHeader.loops[0].dwell =
    //            static_cast<uint16_t>(
    //                loop1DwellEditor.getText().getIntValue()
    //                );

    //        if (onSampleHeaderChanged)
    //            onSampleHeaderChanged(currentHeader);
    //    };

    //addAndMakeVisible(loop1LengthLabel);
    //addAndMakeVisible(loop1LengthEditor);

    loopSelectLabel.setText(
        "Loop",
        juce::dontSendNotification
    );

    loopSelectCombo.addItem("Loop 1", 1);
    loopSelectCombo.addItem("Loop 2", 2);
    loopSelectCombo.addItem("Loop 3", 3);
    loopSelectCombo.addItem("Loop 4", 4);

    loopSelectCombo.onChange = [this]()
        {
            selectedLoopIndex =
                loopSelectCombo.getSelectedId() - 1;

            if (selectedLoopIndex < 0 ||
                selectedLoopIndex >= 4)
                return;

            const auto& loop =
                currentHeader.loops[selectedLoopIndex];

            loopPositionEditor.setText(
                juce::String((juce::int64)loop.position),
                false
            );

            loopLengthEditor.setText(
                juce::String(loop.length, 3),
                false
            );

            loopDwellEditor.setText(
                juce::String(loop.dwell),
                false
            );


            updateLoopDwellStatus();
        };

    //highestLoopEditor.onFocusLost = [this]()
    //    {
    //        const int value =
    //            highestLoopEditor.getText().getIntValue();

    //        if (value < 0 || value > 3)
    //        {
    //            highestLoopEditor.setText(
    //                juce::String(currentHeader.highestLoop),
    //                false
    //            );
    //            return;
    //        }

    //        if (value == currentHeader.highestLoop)
    //            return;

    //        currentHeader.highestLoop = value;

    //        DBG(
    //            "HIGHEST LOOP CHANGED = "
    //            + juce::String(value)
    //        );

    //        if (onSampleHeaderChanged)
    //            onSampleHeaderChanged(currentHeader);
    //    };

    loopPositionLabel.setText(
        "Loop Position",
        juce::dontSendNotification
    );

    loopLengthLabel.setText(
        "Loop Length",
        juce::dontSendNotification
    );

    loopDwellLabel.setText(
        "Loop Dwell",
        juce::dontSendNotification
    );

    loopDwellStatusLabel.setText(
        "",
        juce::dontSendNotification
    );

    playTypeLabel.setText(
        "Playback Type",
        juce::dontSendNotification
    );

    addAndMakeVisible(playTypeLabel);
    addAndMakeVisible(playTypeCombo);

    //playTypeCombo.addItem("Normal Loop", 1);
    //playTypeCombo.addItem("Loop Until Release", 2);
    //playTypeCombo.addItem("No Loop", 3);
    //playTypeCombo.addItem("Play To End", 4);

    playTypeCombo.onChange = [this]()
        {
            const int selectedId =
                playTypeCombo.getSelectedId();

            if (selectedId <= 0)
                return;

            // ComboBox ID 1-4 Å® S3000XL raw 0-3
            const int value = selectedId - 1;

            if (value == currentHeader.playType)
                return;

            currentHeader.playType = value;

            DBG(
                "PLAYBACK TYPE CHANGED = "
                + juce::String(value)
            );

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);
        };


    addAndMakeVisible(loopDwellStatusLabel);


    addAndMakeVisible(loopSelectLabel);
    addAndMakeVisible(loopSelectCombo);

    addAndMakeVisible(loopPositionLabel);
    addAndMakeVisible(loopPositionEditor);

    addAndMakeVisible(loopLengthLabel);
    addAndMakeVisible(loopLengthEditor);

    addAndMakeVisible(loopDwellLabel);
    addAndMakeVisible(loopDwellEditor);



    addAndMakeVisible(sampleRateLabel);
    addAndMakeVisible(holdLoopTuneLabel);

    addAndMakeVisible(sampleRateEditor);
    addAndMakeVisible(holdLoopTuneEditor);




    addAndMakeVisible(idLabel);
    addAndMakeVisible(nameLabel);
    addAndMakeVisible(bandwidthLabel);
    addAndMakeVisible(pitchLabel);
    addAndMakeVisible(sampleRateValidLabel);
    addAndMakeVisible(numLoopsLabel);
    //addAndMakeVisible(activeLoopLabel);
    //addAndMakeVisible(highestLoopLabel);
    addAndMakeVisible(playTypeLabel);
    addAndMakeVisible(tuneLabel);
    addAndMakeVisible(locationLabel);
    addAndMakeVisible(lengthLabel);
    addAndMakeVisible(startLabel);
    addAndMakeVisible(endLabel);
    addAndMakeVisible(loopsLabel);


    // =========================================================
    // Editors
    // =========================================================

    addAndMakeVisible(idEditor);
    addAndMakeVisible(nameEditor);
    //addAndMakeVisible(bandwidthEditor);
    addAndMakeVisible(pitchEditor);
    addAndMakeVisible(sampleRateValidCombo);
    //addAndMakeVisible(numLoopsEditor);
    addAndMakeVisible(numLoopsCombo);
    //addAndMakeVisible(activeLoopEditor);
    //addAndMakeVisible(activeLoopCombo);
    //addAndMakeVisible(highestLoopEditor);
    addAndMakeVisible(playTypeCombo);
    addAndMakeVisible(tuneEditor);
    addAndMakeVisible(locationEditor);
    addAndMakeVisible(lengthEditor);
    addAndMakeVisible(startEditor);
    addAndMakeVisible(endEditor);
    addAndMakeVisible(loopsEditor);


    // =========================================================
    // Editor settings
    // =========================================================

    nameEditor.setMultiLine(false);
    nameEditor.setReadOnly(true);

    loopsEditor.setMultiLine(true);
    loopsEditor.setReadOnly(true);

    idEditor.setReadOnly(true);
    locationEditor.setReadOnly(true);
    //lengthEditor.setReadOnly(true);
    //startEditor.setReadOnly(true);
    //endEditor.setReadOnly(true);

    //sampleRateEditor.setReadOnly(true);
    //holdLoopTuneEditor.setReadOnly(true);

}

void SampleHeaderEditor::updateLoopDwellStatus()
{
    if (selectedLoopIndex < 0 ||
        selectedLoopIndex >= 4)
        return;

    const auto dwell =
        currentHeader.loops[selectedLoopIndex].dwell;

    if (dwell == 0)
    {
        loopDwellStatusLabel.setText(
            "No Loop",
            juce::dontSendNotification
        );
    }
    else if (dwell == 9999)
    {
        loopDwellStatusLabel.setText(
            "Hold",
            juce::dontSendNotification
        );
    }
    else
    {
        loopDwellStatusLabel.setText(
            juce::String(dwell) + " ms",
            juce::dontSendNotification
        );
    }
}

void SampleHeaderEditor::setSampleHeader(
    const SampleHeader& header)
{
    currentHeader = header;
    hasValidHeader = true;

    idEditor.setText(
        juce::String(header.id),
        false
    );

    nameEditor.setText(
        header.name,
        false
    );

    //bandwidthEditor.setText(
    //    juce::String(header.bandwidth),
    //    false
    //);

    sampleRateEditor.setText(
        juce::String(header.sampleRate),
        false
    );


    bandwidthCombo.setSelectedId(
        header.bandwidth + 1,
        juce::dontSendNotification
    );

    pitchEditor.setText(
        juce::String(header.originalPitch),
        false
    );

    pitchNoteLabel.setText(
        midiNoteName(header.originalPitch),
        juce::dontSendNotification
    );

    sampleRateValidCombo.setSelectedId(
        header.sampleRateValid ? 2 : 1,
        juce::dontSendNotification
    );

    //numLoopsEditor.setText(
    //    juce::String(header.numLoops),
    //    false
    //);

    numLoopsCombo.setSelectedId(
        header.numLoops + 1,
        juce::dontSendNotification
    );

    //activeLoopEditor.setText(
    //    juce::String(header.activeLoop),
    //    false
    //);

    activeLoopCombo.setSelectedId(
        header.activeLoop + 1,
        juce::dontSendNotification
    );

    highestLoopEditor.setText(
        juce::String(header.highestLoop),
        false
    );

    playTypeCombo.setSelectedId(
        header.playType + 1,
        juce::dontSendNotification
    );

    tuneEditor.setText(
        juce::String(header.tune),
        false
    );

    locationEditor.setText(
        juce::String(header.location),
        false
    );

    lengthEditor.setText(
        juce::String(header.length),
        false
    );

    startEditor.setText(
        juce::String(header.start),
        false
    );

    endEditor.setText(
        juce::String(header.end),
        false
    );

    sampleRateEditor.setText(
        juce::String(currentHeader.sampleRate),
        false
    );

    holdLoopTuneEditor.setText(
        juce::String((int)currentHeader.holdLoopTune),
        false
    );

    //loop1PositionEditor.setText(
    //    juce::String(
    //        static_cast<juce::int64>(
    //            header.loops[0].position
    //            )
    //    ),
    //    false
    //);



    //loop1LengthEditor.setText(
    //    juce::String(
    //        header.loops[0].length,
    //        3
    //    ),
    //    false
    //);

    //loop1LengthEditor.onFocusLost = [this]()
    //    {
    //        currentHeader.loops[0].length =
    //            loop1LengthEditor.getText().getDoubleValue();

    //        if (onSampleHeaderChanged)
    //            onSampleHeaderChanged(currentHeader);
    //    };

    //loop1DwellEditor.setText(
    //    juce::String(header.loops[0].dwell),
    //    false
    //);




    // =========================================================
    // Loops
    // =========================================================

    juce::String loopsText;

    for (size_t i = 0; i < header.loops.size(); ++i)
    {
        const auto& loop = header.loops[i];

        loopsText +=
            "Loop "
            + juce::String((int)i + 1)
            + "\n";

        loopsText +=
            "  Position: "
            + juce::String(loop.position)
            + "\n";

        loopsText +=
            "  Length: "
            + juce::String(loop.length)
            + "\n";

        loopsText +=
            "  Dwell: "
            + juce::String(loop.dwell)
            + "\n\n";
    }

    loopsEditor.setText(
        loopsText,
        false
    );

    // =========================================================
// Loop selector
// =========================================================

    selectedLoopIndex = 0;

    loopSelectCombo.setSelectedId(
        1,
        juce::dontSendNotification
    );

    const auto& loop =
        currentHeader.loops[selectedLoopIndex];

    loopPositionEditor.setText(
        juce::String((juce::int64)loop.position),
        false
    );

    loopLengthEditor.setText(
        juce::String(loop.length, 3),
        false
    );

    loopDwellEditor.setText(
        juce::String(loop.dwell),
        false
    );

    updateLoopDwellStatus();


}

void SampleHeaderEditor::resized()
{
    auto area = getLocalBounds().reduced(10);

    titleLabel.setBounds(
        area.removeFromTop(30)
    );

    const int rowHeight = 26;
    const int labelWidth = 140;

    auto addRow =
        [&area, rowHeight, labelWidth]
        (
            juce::Label& label,
            juce::TextEditor& editor
            )
        {
            auto row = area.removeFromTop(rowHeight);

            label.setBounds(
                row.removeFromLeft(labelWidth)
            );

            editor.setBounds(row);
        };

    addRow(idLabel, idEditor);
    addRow(nameLabel, nameEditor);
    {
        auto row = area.removeFromTop(rowHeight);

        bandwidthLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        bandwidthCombo.setBounds(row);
    }


    
    addRow(pitchLabel, pitchEditor);
    pitchNoteLabel.setBounds(
        area.removeFromTop(rowHeight)
    );

    //addRow(sampleRateValidLabel, sampleRateValidEditor);
    {
        auto row = area.removeFromTop(rowHeight);

        sampleRateValidLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        sampleRateValidCombo.setBounds(row);
    }

    // í«â¡
    addRow(sampleRateLabel, sampleRateEditor);

    //addRow(numLoopsLabel, numLoopsEditor);
    {
        auto row = area.removeFromTop(rowHeight);

        numLoopsLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        numLoopsCombo.setBounds(row);
    }

    //addRow(activeLoopLabel, activeLoopEditor);

    //{
    //    auto row = area.removeFromTop(rowHeight);

    //    activeLoopLabel.setBounds(
    //        row.removeFromLeft(labelWidth)
    //    );

    //    activeLoopCombo.setBounds(row);
    //}


    //addRow(highestLoopLabel, highestLoopEditor);
    //{
    //    auto row = area.removeFromTop(rowHeight);

    //    playTypeLabel.setBounds(
    //        row.removeFromLeft(labelWidth)
    //    );

    //    playTypeCombo.setBounds(row);
    //}
    addRow(tuneLabel, tuneEditor);

    // í«â¡
    addRow(holdLoopTuneLabel, holdLoopTuneEditor);

    addRow(locationLabel, locationEditor);
    addRow(lengthLabel, lengthEditor);
    addRow(startLabel, startEditor);
    addRow(endLabel, endEditor);

    {
        auto row = area.removeFromTop(rowHeight);

        loopSelectLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        loopSelectCombo.setBounds(row);
    }

    addRow(
        loopPositionLabel,
        loopPositionEditor
    );

    addRow(
        loopLengthLabel,
        loopLengthEditor
    );

    addRow(
        loopDwellLabel,
        loopDwellEditor
    );

    loopDwellStatusLabel.setBounds(
        area.removeFromTop(rowHeight)
    );


    //loopsLabel.setBounds(
    //    area.removeFromTop(rowHeight)
    //);

    {
        auto row = area.removeFromTop(rowHeight);

        playTypeLabel.setBounds(
            row.removeFromLeft(labelWidth)
        );

        playTypeCombo.setBounds(row);
    }


    /*loopsEditor.setBounds(area);*/
}

juce::String SampleHeaderEditor::midiNoteName(int note)
{
    static const char* names[] =
    {
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    };

    if (note < 0 || note > 127)
        return "-";

    const int octave = (note / 12) - 1;
    const int noteIndex = note % 12;

    return juce::String(names[noteIndex])
        + juce::String(octave);
}