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
    addAndMakeVisible(samplePositionBar);

    samplePositionBar.onRangeChanged =
        [this](
            uint32_t start,
            uint32_t end)
        {
            if (!hasValidHeader)
                return;

            currentHeader.start = start;
            currentHeader.end = end;

            auto& loop =
                currentHeader.loops[selectedLoopIndex];

            // ==============================
            // START側の制約
            // ==============================

            if (loop.position < currentHeader.start)
            {
                const double oldLoopEnd =
                    static_cast<double>(loop.position)
                    + loop.length;

                loop.position =
                    currentHeader.start;

                if (oldLoopEnd > currentHeader.start)
                {
                    loop.length =
                        oldLoopEnd
                        - static_cast<double>(
                            currentHeader.start
                            );
                }
                else
                {
                    loop.length = 0.0;
                }
            }

            // ==============================
            // END側の制約
            // ==============================

            const double loopEnd =
                static_cast<double>(loop.position)
                + loop.length;

            if (loopEnd > currentHeader.end)
            {
                if (loop.position <= currentHeader.end)
                {
                    loop.length =
                        static_cast<double>(
                            currentHeader.end
                            - loop.position
                            );
                }
                else
                {
                    loop.position =
                        currentHeader.end;

                    loop.length = 0.0;
                }
            }

            // ==============================
            // 数値欄更新
            // ==============================

            startEditor.setText(
                juce::String(currentHeader.start),
                false
            );

            endEditor.setText(
                juce::String(currentHeader.end),
                false
            );

            loopPositionEditor.setText(
                juce::String(
                    (juce::int64)loop.position
                ),
                false
            );

            loopLengthEditor.setText(
                juce::String(
                    loop.length,
                    3
                ),
                false
            );

            // ==============================
            // バー更新
            // ==============================

            samplePositionBar.setPositions(
                currentHeader.length,
                currentHeader.start,
                currentHeader.end,
                loop.position,
                static_cast<uint32_t>(
                    loop.position
                    + loop.length
                    )
            );
        };


    samplePositionBar.onLoopRangeChanged =
        [this](
            uint32_t loopStart,
            uint32_t loopEnd)
        {
            if (!hasValidHeader)
                return;

            auto& loop =
                currentHeader.loops[selectedLoopIndex];

            loop.position = loopStart;

            loop.length =
                static_cast<double>(
                    loopEnd - loopStart
                    );

            loopPositionEditor.setText(
                juce::String(
                    (juce::int64)loop.position
                ),
                false
            );

            loopLengthEditor.setText(
                juce::String(
                    loop.length,
                    3
                ),
                false
            );

            //if (onSampleHeaderChanged)
            //{
            //    onSampleHeaderChanged(
            //        currentHeader
            //    );
            //}
        };


    samplePositionBar.onEditFinished =
        [this]()
        {
            if (!hasValidHeader)
                return;

            if (onSampleHeaderChanged)
            {
                onSampleHeaderChanged(
                    currentHeader
                );
            }
        };



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

    //        // Bandwidth は 0 / 1 のみ
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
            // ComboBox ID は1始まりなので -1
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
            if (!hasValidHeader)
                return;

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

            const auto& loop =
                currentHeader.loops[selectedLoopIndex];

            samplePositionBar.setPositions(
                currentHeader.length,
                currentHeader.start,
                currentHeader.end,
                loop.position,
                static_cast<uint32_t>(
                    loop.position + loop.length
                    )
            );

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

            const auto& loop =
                currentHeader.loops[selectedLoopIndex];

            samplePositionBar.setPositions(
                currentHeader.length,
                currentHeader.start,
                currentHeader.end,
                loop.position,
                static_cast<uint32_t>(
                    loop.position + loop.length
                    )
            );

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

            // Loop EndがSample Endを超えない
            if (newPosition > currentHeader.end ||
                static_cast<double>(newPosition) + loop.length >
                static_cast<double>(currentHeader.end))
            {
                loopPositionEditor.setText(
                    juce::String(
                        (juce::int64)loop.position
                    ),
                    false
                );

                return;
            }

            // Model更新
            currentHeader.loops[selectedLoopIndex].position =
                newPosition;

            const auto& updatedLoop =
                currentHeader.loops[selectedLoopIndex];

            const auto loopEnd =
                static_cast<uint32_t>(
                    updatedLoop.position
                    + updatedLoop.length
                    );

            // オレンジのLoop範囲も更新
            samplePositionBar.setPositions(
                currentHeader.length,
                currentHeader.start,
                currentHeader.end,
                updatedLoop.position,
                loopEnd
            );

            // 実機へ反映
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
                static_cast<double>(currentHeader.end))
            {
                loopLengthEditor.setText(
                    juce::String(loop.length, 3),
                    false
                );
                return;
            }

            currentHeader.loops[selectedLoopIndex].length =
                newLength;

            const auto& updatedLoop =
                currentHeader.loops[selectedLoopIndex];

            samplePositionBar.setPositions(
                currentHeader.length,
                currentHeader.start,
                currentHeader.end,
                updatedLoop.position,
                static_cast<uint32_t>(
                    updatedLoop.position
                    + updatedLoop.length
                    )
            );

            if (onSampleHeaderChanged)
                onSampleHeaderChanged(currentHeader);
        };

    sampleRateEditor.onFocusLost = [this]()
        {
            const int value =
                sampleRateEditor.getText().getIntValue();

            // 16-bit fieldなので最低限この範囲
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

            // ComboBox ID 1 → S3000XL value 0
            // ComboBox ID 2 → S3000XL value 1
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

            samplePositionBar.setPositions(
                currentHeader.length,
                currentHeader.start,
                currentHeader.end,
                loop.position,
                static_cast<uint32_t>(
                    loop.position + loop.length
                    )
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

            // ComboBox ID 1-4 → S3000XL raw 0-3
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
    //addAndMakeVisible(locationLabel);
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
    //addAndMakeVisible(locationEditor);
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
        sampleSectionLabel,
        "SAMPLE"
    );

    setupSectionLabel(
        pitchSectionLabel,
        "PITCH"
    );

    setupSectionLabel(
        rangeSectionLabel,
        "RANGE"
    );

    setupSectionLabel(
        loopSectionLabel,
        "LOOP"
    );


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

    //locationEditor.setText(
    //    juce::String(header.location),
    //    false
    //);

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

    const auto loopStart =
        header.loops[0].position;

    const auto loopEnd =
        static_cast<uint32_t>(
            header.loops[0].position
            + header.loops[0].length
            );

    samplePositionBar.setPositions(
        header.length,
        header.start,
        header.end,
        loopStart,
        loopEnd
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

void SampleHeaderEditor::paint(
    juce::Graphics& g)
{
    auto drawCard =
        [&g](const juce::Rectangle<int>& bounds)
        {
            if (bounds.isEmpty())
                return;

            auto r =
                bounds.toFloat();

            g.setColour(
                juce::Colours::white
                .withAlpha(0.045f)
            );

            g.fillRoundedRectangle(
                r,
                6.0f
            );

            g.setColour(
                juce::Colours::white
                .withAlpha(0.10f)
            );

            g.drawRoundedRectangle(
                r,
                6.0f,
                1.0f
            );
        };

    drawCard(sampleCardBounds);
    drawCard(pitchCardBounds);
    drawCard(rangeCardBounds);
    drawCard(loopCardBounds);
}

void SampleHeaderEditor::resized()
{
    auto area =
        getLocalBounds().reduced(10);

    const int rowHeight = 26;
    const int sectionHeight = 22;
    const int labelWidth = 110;

    constexpr int cardPadding = 10;
    constexpr int columnGap = 14;
    constexpr int cardGap = 10;

    // ========================================
    // TITLE
    // ========================================

    titleLabel.setBounds(
        area.removeFromTop(30)
    );

    area.removeFromTop(6);

    // ========================================
    // SAMPLE + PITCH
    // ========================================

    const int sampleCardHeight =
        sectionHeight
        + rowHeight * 5
        + cardPadding * 2;

    const int pitchCardHeight =
        sectionHeight
        + rowHeight * 4
        + cardPadding * 2;

    auto topRow =
        area.removeFromTop(
            juce::jmax(
                sampleCardHeight,
                pitchCardHeight
            )
        );

    const int leftWidth =
        static_cast<int>(
            topRow.getWidth() * 0.50f
            )
        - columnGap / 2;

    auto topLeft =
        topRow.removeFromLeft(leftWidth);

    topRow.removeFromLeft(columnGap);

    auto topRight = topRow;

    sampleCardBounds =
        topLeft.removeFromTop(
            sampleCardHeight
        );

    pitchCardBounds =
        topRight.removeFromTop(
            pitchCardHeight
        );

    area.removeFromTop(cardGap);

    // ========================================
    // RANGE CARD
    // ========================================

    const int rangeCardHeight =
        sectionHeight
        + rowHeight
        + 80
        + cardPadding * 2;

    rangeCardBounds =
        area.removeFromTop(
            rangeCardHeight
        );

    area.removeFromTop(cardGap);

    // ========================================
    // LOOP CARD
    // ========================================

    const int loopCardHeight =
        sectionHeight
        + rowHeight * 5
        + cardPadding * 2;

    loopCardBounds =
        area.removeFromTop(
            loopCardHeight
        );

    // ========================================
    // ROW HELPER
    // ========================================

    auto addRow =
        [rowHeight, labelWidth]
        (
            juce::Rectangle<int>& column,
            juce::Label& label,
            auto& editor
            )
        {
            auto row =
                column.removeFromTop(
                    rowHeight
                );

            label.setBounds(
                row.removeFromLeft(
                    labelWidth
                )
            );

            editor.setBounds(row);
        };

    // ========================================
    // SAMPLE CARD
    // ========================================

    auto sampleArea =
        sampleCardBounds.reduced(
            cardPadding
        );

    sampleSectionLabel.setBounds(
        sampleArea.removeFromTop(
            sectionHeight
        )
    );

    addRow(
        sampleArea,
        nameLabel,
        nameEditor
    );

    addRow(
        sampleArea,
        idLabel,
        idEditor
    );

    addRow(
        sampleArea,
        lengthLabel,
        lengthEditor
    );

    addRow(
        sampleArea,
        sampleRateLabel,
        sampleRateEditor
    );

    {
        auto row =
            sampleArea.removeFromTop(
                rowHeight
            );

        bandwidthLabel.setBounds(
            row.removeFromLeft(
                labelWidth
            )
        );

        bandwidthCombo.setBounds(row);
    }

    // ========================================
    // PITCH CARD
    // ========================================

    auto pitchArea =
        pitchCardBounds.reduced(
            cardPadding
        );

    pitchSectionLabel.setBounds(
        pitchArea.removeFromTop(
            sectionHeight
        )
    );

    addRow(
        pitchArea,
        pitchLabel,
        pitchEditor
    );

    pitchNoteLabel.setBounds(
        pitchArea.removeFromTop(
            rowHeight
        )
    );

    addRow(
        pitchArea,
        tuneLabel,
        tuneEditor
    );

    addRow(
        pitchArea,
        holdLoopTuneLabel,
        holdLoopTuneEditor
    );

    // ========================================
    // RANGE CARD
    // ========================================

    auto rangeArea =
        rangeCardBounds.reduced(
            cardPadding
        );

    rangeSectionLabel.setBounds(
        rangeArea.removeFromTop(
            sectionHeight
        )
    );

    {
        auto row =
            rangeArea.removeFromTop(
                rowHeight
            );

        auto left =
            row.removeFromLeft(
                row.getWidth() / 2
            );

        startLabel.setBounds(
            left.removeFromLeft(
                labelWidth
            )
        );

        startEditor.setBounds(left);

        endLabel.setBounds(
            row.removeFromLeft(
                labelWidth
            )
        );

        endEditor.setBounds(row);
    }

    samplePositionBar.setBounds(
        rangeArea.removeFromTop(80)
        .reduced(5)
    );

    // ========================================
    // LOOP CARD
    // ========================================

    auto loopArea =
        loopCardBounds.reduced(
            cardPadding
        );

    loopSectionLabel.setBounds(
        loopArea.removeFromTop(
            sectionHeight
        )
    );

    {
        auto row =
            loopArea.removeFromTop(
                rowHeight
            );

        auto left =
            row.removeFromLeft(
                row.getWidth() / 2
            );

        numLoopsLabel.setBounds(
            left.removeFromLeft(
                labelWidth
            )
        );

        numLoopsCombo.setBounds(left);

        loopSelectLabel.setBounds(
            row.removeFromLeft(
                labelWidth
            )
        );

        loopSelectCombo.setBounds(row);
    }

    addRow(
        loopArea,
        loopPositionLabel,
        loopPositionEditor
    );

    addRow(
        loopArea,
        loopLengthLabel,
        loopLengthEditor
    );

    {
        auto row =
            loopArea.removeFromTop(
                rowHeight
            );

        loopDwellLabel.setBounds(
            row.removeFromLeft(
                labelWidth
            )
        );

        auto statusArea =
            row.removeFromRight(80);

        loopDwellEditor.setBounds(row);

        loopDwellStatusLabel.setBounds(
            statusArea
        );
    }

    {
        auto row =
            loopArea.removeFromTop(
                rowHeight
            );

        playTypeLabel.setBounds(
            row.removeFromLeft(
                labelWidth
            )
        );

        playTypeCombo.setBounds(row);
    }
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

void SampleHeaderEditor::clear()
{
    hasValidHeader = false;

    idEditor.clear();
    nameEditor.clear();
    sampleRateEditor.clear();
    pitchEditor.clear();
    //pitchNoteLabel.clear();
    pitchNoteLabel.setText(
        "",
        juce::dontSendNotification
    );

    highestLoopEditor.clear();
    tuneEditor.clear();
    locationEditor.clear();
    lengthEditor.clear();
    startEditor.clear();
    endEditor.clear();
    holdLoopTuneEditor.clear();

    loopsEditor.clear();

    loopPositionEditor.clear();
    loopLengthEditor.clear();
    loopDwellEditor.clear();

    bandwidthCombo.setSelectedId(
        0,
        juce::dontSendNotification
    );

    sampleRateValidCombo.setSelectedId(
        0,
        juce::dontSendNotification
    );

    numLoopsCombo.setSelectedId(
        0,
        juce::dontSendNotification
    );

    activeLoopCombo.setSelectedId(
        0,
        juce::dontSendNotification
    );

    playTypeCombo.setSelectedId(
        0,
        juce::dontSendNotification
    );

    loopSelectCombo.setSelectedId(
        0,
        juce::dontSendNotification
    );

    selectedLoopIndex = 0;

    DBG("SAMPLE HEADER EDITOR CLEARED");
}