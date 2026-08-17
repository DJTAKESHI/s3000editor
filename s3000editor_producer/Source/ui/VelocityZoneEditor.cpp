#include "VelocityZoneEditor.h"

VelocityZoneEditor::VelocityZoneEditor()
{
    titleLabel.setText(
        "Velocity Zone",
        juce::dontSendNotification
    );

    titleLabel.setFont(
        juce::Font(18.0f, juce::Font::bold)
    );

    sampleNameLabel.setText("Sample", juce::dontSendNotification);
    sampleIdLabel.setText("Sample ID", juce::dontSendNotification);
    lowVelLabel.setText("Low Velocity", juce::dontSendNotification);
    highVelLabel.setText("High Velocity", juce::dontSendNotification);
    //tuneLabel.setText("Tune", juce::dontSendNotification);
    loudnessLabel.setText("Loudness", juce::dontSendNotification);
    panLabel.setText("Pan", juce::dontSendNotification);

    playModeLabel.setText(
        "Play Mode",
        juce::dontSendNotification
    );

    //playModeCombo.addItem("As Sample", 1);
    //playModeCombo.addItem("Normal Loop", 2);
    //playModeCombo.addItem("Loop Until Release", 3);
    //playModeCombo.addItem("No Loop", 4);
    //playModeCombo.addItem("Play To End", 5);

    addAndMakeVisible(playModeLabel);
    addAndMakeVisible(playModeCombo);

    playModeCombo.onChange = [this]()
        {
            const int value =
                playModeCombo.getSelectedId() - 1;

            currentZone.playMode =
                static_cast<PlayMode>(value);

            if (onZoneChanged)
                onZoneChanged(currentZone);
        };


    addAndMakeVisible(titleLabel);

    addAndMakeVisible(sampleNameLabel);
    //addAndMakeVisible(sampleNameEditor);
    addAndMakeVisible(sampleCombo);

    addAndMakeVisible(sampleIdLabel);
    addAndMakeVisible(sampleIdEditor);

    addAndMakeVisible(lowVelLabel);
    addAndMakeVisible(lowVelEditor);

    addAndMakeVisible(highVelLabel);
    addAndMakeVisible(highVelEditor);

    /*addAndMakeVisible(tuneLabel);
    addAndMakeVisible(tuneEditor);*/

    semitoneLabel.setText(
        "SEM",
        juce::dontSendNotification
    );

    fineTuneLabel.setText(
        "CNT",
        juce::dontSendNotification
    );

    addAndMakeVisible(semitoneLabel);
    addAndMakeVisible(semitoneEditor);

    addAndMakeVisible(fineTuneLabel);
    addAndMakeVisible(fineTuneEditor);



    addAndMakeVisible(loudnessLabel);
    addAndMakeVisible(loudnessEditor);

    addAndMakeVisible(panLabel);
    addAndMakeVisible(panEditor);

    // まず表示だけ
    //sampleNameEditor.setReadOnly(true);
    sampleIdEditor.setReadOnly(true);

    lowVelEditor.onFocusLost = [this]()
        {
            const int value =
                lowVelEditor.getText().getIntValue();

            if (value < 0 || value > 127)
            {
                lowVelEditor.setText(
                    juce::String(currentZone.lowVel),
                    false
                );
                return;
            }

            currentZone.lowVel = value;

            if (onZoneChanged)
                onZoneChanged(currentZone);
        };


    highVelEditor.onFocusLost = [this]()
        {
            const int value =
                highVelEditor.getText().getIntValue();

            if (value < 0 || value > 127)
            {
                highVelEditor.setText(
                    juce::String(currentZone.highVel),
                    false
                );
                return;
            }

            currentZone.highVel = value;

            if (onZoneChanged)
                onZoneChanged(currentZone);
        };

    //sampleNameEditor.setReadOnly(true);
    //sampleNameEditor.onFocusLost = [this]()
    //    {
    //        const auto newName =
    //            sampleNameEditor.getText();

    //        if (newName == currentZone.sampleName)
    //            return;

    //        currentZone.sampleName = newName;

    //        // 名前変更時点ではIDは未解決に戻す
    //        currentZone.sampleId = -1;

    //        if (onZoneChanged)
    //            onZoneChanged(currentZone);
    //    };


    //tuneEditor.onFocusLost = [this]()
    //    {
    //        const int value =
    //            tuneEditor.getText().getIntValue();

    //        // ひとまず signed 8-bit 範囲
    //        if (value < -128 || value > 127)
    //        {
    //            tuneEditor.setText(
    //                juce::String(currentZone.tune),
    //                false
    //            );
    //            return;
    //        }

    //        currentZone.tune = value;

    //        if (onZoneChanged)
    //            onZoneChanged(currentZone);
    //    };
    sampleCombo.onChange = [this]()
        {
            const int selectedId =
                sampleCombo.getSelectedId() - 1;

            if (selectedId < 0 ||
                selectedId >= (int)residentSamples.size())
            {
                return;
            }

            if (currentZone.sampleId == selectedId)
                return;

            currentZone.sampleId =
                selectedId;

            currentZone.sampleName =
                residentSamples[selectedId];

            DBG(
                "SAMPLE COMBO CHANGED ID="
                + juce::String(selectedId)
                + " NAME=["
                + currentZone.sampleName
                + "]"
            );

            if (onZoneChanged)
            {
                onZoneChanged(currentZone);
            }
        };

    semitoneEditor.onFocusLost = [this]()
        {
            const int value =
                semitoneEditor.getText().getIntValue();

            // ひとまずsigned byteより安全な範囲
            if (value < -50 || value > 50)
            {
                semitoneEditor.setText(
                    juce::String(currentZone.semitone),
                    false
                );
                return;
            }

            currentZone.semitone = value;

            if (onZoneChanged)
                onZoneChanged(currentZone);
        };

    fineTuneEditor.onFocusLost = [this]()
        {
            const int cents =
                fineTuneEditor.getText().getIntValue();

            if (cents < -50 || cents > 50)
            {
                const int currentCents =
                    juce::roundToInt(
                        currentZone.fineTuneRaw
                        * 100.0
                        / 256.0
                    );

                fineTuneEditor.setText(
                    juce::String(currentCents),
                    false
                );

                return;
            }

            currentZone.fineTuneRaw =
                juce::roundToInt(
                    cents * 256.0 / 100.0
                );

            if (onZoneChanged)
                onZoneChanged(currentZone);
        };



    loudnessEditor.onFocusLost = [this]()
        {
            const int value =
                loudnessEditor.getText().getIntValue();

            if (value < -128 || value > 127)
            {
                loudnessEditor.setText(
                    juce::String(currentZone.loudness),
                    false
                );
                return;
            }

            currentZone.loudness = value;

            if (onZoneChanged)
                onZoneChanged(currentZone);
        };


    panEditor.onFocusLost = [this]()
        {
            const int value =
                panEditor.getText().getIntValue();

            if (value < -128 || value > 127)
            {
                panEditor.setText(
                    juce::String(currentZone.pan),
                    false
                );
                return;
            }

            currentZone.pan = value;

            if (onZoneChanged)
                onZoneChanged(currentZone);
        };

    playModeLabel.setText(
        "Play Mode",
        juce::dontSendNotification
    );


    filterFreqEditor.onFocusLost = [this]()
        {
            const int value =
                filterFreqEditor.getText().getIntValue();

            if (value < -50 || value > 50)
                return;

            if (value == currentZone.filterFreq)
                return;

            currentZone.filterFreq = value;

            if (onZoneChanged)
                onZoneChanged(currentZone);
        };

    //lowVelXFadeEditor.onFocusLost = [this]()
    //    {
    //        const int value =
    //            lowVelXFadeEditor.getText().getIntValue();

    //        if (value < 0 || value > 255)
    //            return;

    //        if (value == currentZone.lowVelXFade)
    //            return;

    //        currentZone.lowVelXFade =
    //            static_cast<uint8_t>(value);

    //        if (onZoneChanged)
    //            onZoneChanged(currentZone);
    //    };

    //highVelXFadeEditor.onFocusLost = [this]()
    //    {
    //        const int value =
    //            highVelXFadeEditor.getText().getIntValue();

    //        if (value < 0 || value > 255)
    //            return;

    //        if (value == currentZone.highVelXFade)
    //            return;

    //        currentZone.highVelXFade =
    //            static_cast<uint8_t>(value);

    //        if (onZoneChanged)
    //            onZoneChanged(currentZone);
    //    };

    playModeCombo.addItem("As Sample", 1);
    playModeCombo.addItem("Normal Loop", 2);
    playModeCombo.addItem("Loop Until Release", 3);
    playModeCombo.addItem("No Loop", 4);
    playModeCombo.addItem("Play To End", 5);

    addAndMakeVisible(playModeLabel);
    addAndMakeVisible(playModeCombo);

    filterFreqLabel.setText(
        "Filter Freq",
        juce::dontSendNotification
    );

    //lowVelXFadeLabel.setText(
    //    "Low Vel XFade",
    //    juce::dontSendNotification
    //);

    //highVelXFadeLabel.setText(
    //    "High Vel XFade",
    //    juce::dontSendNotification
    //);

    addAndMakeVisible(filterFreqLabel);
    addAndMakeVisible(filterFreqEditor);

    //addAndMakeVisible(lowVelXFadeLabel);
    //addAndMakeVisible(lowVelXFadeEditor);

    //addAndMakeVisible(highVelXFadeLabel);
    //addAndMakeVisible(highVelXFadeEditor);


}


void VelocityZoneEditor::setZone(
    const VelocityZone& zone)
{
    DBG("===== VelocityZoneEditor::setZone =====");

    DBG(
        "SAMPLE = ["
        + zone.sampleName
        + "]"
    );

    DBG(
        "ID = "
        + juce::String(zone.sampleId)
    );

    DBG(
        "LOW = "
        + juce::String(zone.lowVel)
    );

    DBG(
        "HIGH = "
        + juce::String(zone.highVel)
    );

    DBG(
        "SEM = "
        + juce::String(zone.semitone)
    );

    DBG(
        "FINE RAW = "
        + juce::String(zone.fineTuneRaw)
    );

    DBG(
        "LOUD = "
        + juce::String(zone.loudness)
    );

    DBG(
        "PAN = "
        + juce::String(zone.pan)
    );
    
    currentZone = zone;

    //sampleNameEditor.setText(
    //    zone.sampleName,
    //    false
    //);

    if (zone.sampleId >= 0)
    {
        sampleCombo.setSelectedId(
            zone.sampleId + 1,
            juce::dontSendNotification
        );
    }
    else
    {
        sampleCombo.setSelectedId(
            0,
            juce::dontSendNotification
        );
    }


    sampleIdEditor.setText(
        juce::String(zone.sampleId),
        false
    );

    lowVelEditor.setText(
        juce::String(zone.lowVel),
        false
    );

    highVelEditor.setText(
        juce::String(zone.highVel),
        false
    );

    /*tuneEditor.setText(
        juce::String(zone.tune),
        false
    );*/

    semitoneEditor.setText(
        juce::String(zone.semitone),
        false
    );

    const int cents =
        juce::roundToInt(
            zone.fineTuneRaw
            * 100.0
            / 256.0
        );

    fineTuneEditor.setText(
        juce::String(cents),
        false
    );


    loudnessEditor.setText(
        juce::String(zone.loudness),
        false
    );

    panEditor.setText(
        juce::String(zone.pan),
        false
    );

    playModeCombo.setSelectedId(
        static_cast<int>(zone.playMode) + 1,
        juce::dontSendNotification
    );

    filterFreqEditor.setText(
        juce::String(zone.filterFreq),
        false
    );

    //lowVelXFadeEditor.setText(
    //    juce::String(zone.lowVelXFade),
    //    false
    //);

    //highVelXFadeEditor.setText(
    //    juce::String(zone.highVelXFade),
    //    false
    //);




}

void VelocityZoneEditor::resized()
{
    auto area = getLocalBounds().reduced(10);

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

    // ★ Sample Combo
    addRow(sampleNameLabel, sampleCombo);

    addRow(sampleIdLabel, sampleIdEditor);
    addRow(lowVelLabel, lowVelEditor);
    addRow(highVelLabel, highVelEditor);

    addRow(semitoneLabel, semitoneEditor);
    addRow(fineTuneLabel, fineTuneEditor);

    addRow(loudnessLabel, loudnessEditor);
    addRow(panLabel, panEditor);
    addRow(playModeLabel, playModeCombo);

    addRow(filterFreqLabel, filterFreqEditor);
    //addRow(lowVelXFadeLabel, lowVelXFadeEditor);
    //addRow(highVelXFadeLabel, highVelXFadeEditor);
}

void VelocityZoneEditor::setResidentSamples(
    const std::map<int, juce::String>& samples)
{
    DBG("=== VelocityZoneEditor::setResidentSamples ===");

    residentSamples = samples;

    sampleCombo.clear(
        juce::dontSendNotification
    );

    for (const auto& [id, name] : residentSamples)
    {
        sampleCombo.addItem(
            name,
            id + 1
        );
    }

    if (currentZone.sampleId >= 0)
    {
        sampleCombo.setSelectedId(
            currentZone.sampleId + 1,
            juce::dontSendNotification
        );
    }

    DBG(
        "SAMPLE COMBO COUNT = "
        + juce::String(sampleCombo.getNumItems())
    );
}