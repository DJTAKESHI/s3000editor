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
    addAndMakeVisible(
        velocityZoneMap
    );

    addAndMakeVisible(semitoneLabel);
    addAndMakeVisible(semitoneEditor);

    addAndMakeVisible(fineTuneLabel);
    addAndMakeVisible(fineTuneEditor);



    addAndMakeVisible(loudnessLabel);
    addAndMakeVisible(loudnessEditor);

    addAndMakeVisible(panLabel);
    addAndMakeVisible(panEditor);

    addAndMakeVisible(velocityRangeBar);

    velocityRangeBar.onRangeChanged =
        [this](int low, int high)
        {
            currentZone.lowVel =
                low;

            currentZone.highVel =
                high;

            // 数値表示も追従
            lowVelEditor.setText(
                juce::String(low),
                juce::dontSendNotification
            );

            highVelEditor.setText(
                juce::String(high),
                juce::dontSendNotification
            );

            if (onZoneChanged)
            {
                onZoneChanged(
                    currentZone
                );
            }
        };

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

            // バーも更新
            velocityRangeBar.setRange(
                currentZone.lowVel,
                currentZone.highVel
            );

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

            // バーも更新
            velocityRangeBar.setRange(
                currentZone.lowVel,
                currentZone.highVel
            );

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
            const int comboId =
                sampleCombo.getSelectedId();

            // ========================================
            // None → ZoneからSampleを解除
            // ========================================

            if (comboId == noneSampleComboId)
            {
                if (currentZone.sampleId == -1 &&
                    currentZone.sampleName.trim().isEmpty())
                {
                    return;
                }

                currentZone.sampleId = -1;
                currentZone.sampleName = "";

                DBG("SAMPLE CLEARED");

                if (onZoneChanged)
                {
                    onZoneChanged(currentZone);
                }

                return;
            }

            // ========================================
            // 通常のSample選択
            // ========================================

            const int selectedId =
                comboId - 1;

            const auto it =
                residentSamples.find(selectedId);

            if (it == residentSamples.end())
            {
                DBG(
                    "INVALID SAMPLE ID="
                    + juce::String(selectedId)
                );

                return;
            }

            if (currentZone.sampleId == selectedId)
                return;

            currentZone.sampleId =
                selectedId;

            currentZone.sampleName =
                it->second;

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

    velocityZoneMap.onZoneSelected =
        [this](int zoneIndex)
        {
            velocityZoneMap.setSelectedZone(
                zoneIndex
            );

            if (onZoneSelected)
            {
                onZoneSelected(
                    zoneIndex
                );
            }
        };

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

void VelocityZoneEditor::setSelectedZone(
    int index)
{
    velocityZoneMap.setSelectedZone(
        index
    );
}

void VelocityZoneEditor::setZones(
    const std::array<VelocityZone, 4>& zones)
{
    DBG("===== VelocityZoneEditor::setZones CALLED =====");

    for (int i = 0; i < 4; ++i)
    {
        DBG(
            "SET ZONE "
            + juce::String(i)
            + " SAMPLE=["
            + zones[i].sampleName
            + "] LOW="
            + juce::String(zones[i].lowVel)
            + " HIGH="
            + juce::String(zones[i].highVel)
        );
    }

    velocityZoneMap.setZones(
        zones
    );
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

    velocityRangeBar.setRange(
        currentZone.lowVel,
        currentZone.highVel
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

    //velocityRangeBar.setRange(
    //    zone.lowVel,
    //    zone.highVel
    //);


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
    auto area =
        getLocalBounds().reduced(10);

    titleLabel.setBounds(
        area.removeFromTop(30)
    );

    // ========================================
    // 4 Zone Overview
    // ========================================

    velocityZoneMap.setBounds(
        area.removeFromTop(130)
    );

    area.removeFromTop(8);

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

    // Sample
    addRow(
        sampleNameLabel,
        sampleCombo
    );

    addRow(
        sampleIdLabel,
        sampleIdEditor
    );

    // Velocity
    addRow(
        lowVelLabel,
        lowVelEditor
    );

    addRow(
        highVelLabel,
        highVelEditor
    );

    area.removeFromTop(6);

    // ========================================
    // Selected Zone Velocity Range
    // ========================================

    velocityRangeBar.setBounds(
        area.removeFromTop(60)
    );

    area.removeFromTop(6);

    // Tune
    addRow(
        semitoneLabel,
        semitoneEditor
    );

    addRow(
        fineTuneLabel,
        fineTuneEditor
    );

    // その他
    addRow(
        loudnessLabel,
        loudnessEditor
    );

    addRow(
        panLabel,
        panEditor
    );

    addRow(
        playModeLabel,
        playModeCombo
    );

    addRow(
        filterFreqLabel,
        filterFreqEditor
    );
}

void VelocityZoneEditor::setResidentSamples(
    const std::map<int, juce::String>& samples)
{
    DBG("=== VelocityZoneEditor::setResidentSamples ===");

    residentSamples = samples;

    sampleCombo.clear(
        juce::dontSendNotification
    );

    // Sample解除用
    sampleCombo.addItem(
        "--- None ---",
        noneSampleComboId
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