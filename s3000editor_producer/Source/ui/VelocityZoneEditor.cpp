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
    velocityZoneMap.onZoneRangeChanged =
        [this](
            int zoneIndex,
            int low,
            int high
            )
        {
            if (onZoneRangeChanged)
            {
                onZoneRangeChanged(
                    zoneIndex,
                    low,
                    high
                );
            }
        };


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

    auto setupSectionLabel =
        [this](juce::Label& label,
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
        velocitySectionLabel,
        "VELOCITY"
    );

    setupSectionLabel(
        pitchSectionLabel,
        "PITCH"
    );

    setupSectionLabel(
        outputSectionLabel,
        "OUTPUT"
    );

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

void VelocityZoneEditor::paint(
    juce::Graphics& g)
{
    auto drawCard =
        [&g](const juce::Rectangle<int>& bounds)
        {
            if (bounds.isEmpty())
                return;

            auto r =
                bounds.toFloat();

            // Card background
            g.setColour(
                juce::Colours::white
                .withAlpha(0.045f)
            );

            g.fillRoundedRectangle(
                r,
                6.0f
            );

            // Subtle border
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
    drawCard(velocityCardBounds);
    drawCard(pitchCardBounds);
    drawCard(outputCardBounds);
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

    const int rowHeight = 26;
    const int sectionHeight = 22;
    const int labelWidth = 90;

    constexpr int cardPadding = 10;
    constexpr int columnGap = 14;
    constexpr int cardGap = 10;

    // ========================================
    // TITLE
    // ========================================

    titleLabel.setBounds(
        area.removeFromTop(30)
    );

    // ========================================
    // 4-ZONE OVERVIEW
    // ========================================

    velocityZoneMap.setBounds(
        area.removeFromTop(100)
    );

    area.removeFromTop(10);



    // ========================================
    // CARD HEIGHTS
    // ========================================

    const int sampleCardHeight =
        sectionHeight
        + rowHeight * 2
        + cardPadding * 2;

    const int pitchCardHeight =
        sectionHeight
        + rowHeight * 2
        + cardPadding * 2;

    const int velocityCardHeight =
        sectionHeight
        + 45
        + rowHeight * 2
        + cardPadding * 2;

    const int outputCardHeight =
        sectionHeight
        + rowHeight * 4
        + cardPadding * 2;

    // ========================================
// CARD AREA
// ========================================

    auto cardArea =
        area;

    // 45 / 55
    const int leftWidth =
        static_cast<int>(
            cardArea.getWidth() * 0.45f
            );

    // ========================================
    // TOP ROW
    // ========================================

    auto topRow =
        cardArea.removeFromTop(
            juce::jmax(
                sampleCardHeight,
                velocityCardHeight
            )
        );

    auto topLeft =
        topRow.removeFromLeft(
            leftWidth
        );

    topRow.removeFromLeft(
        columnGap
    );

    auto topRight =
        topRow;

    sampleCardBounds =
        topLeft.removeFromTop(
            sampleCardHeight
        );

    velocityCardBounds =
        topRight.removeFromTop(
            velocityCardHeight
        );

    cardArea.removeFromTop(
        cardGap
    );

    // ========================================
    // BOTTOM ROW
    // ========================================

    auto bottomRow =
        cardArea;

    auto bottomLeft =
        bottomRow.removeFromLeft(
            leftWidth
        );

    bottomRow.removeFromLeft(
        columnGap
    );

    auto bottomRight =
        bottomRow;

    pitchCardBounds =
        bottomLeft.removeFromTop(
            pitchCardHeight
        );

    outputCardBounds =
        bottomRight.removeFromTop(
            outputCardHeight
        );

    // ========================================
    // Helper
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

            editor.setBounds(
                row
            );
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
        sampleNameLabel,
        sampleCombo
    );

    addRow(
        sampleArea,
        sampleIdLabel,
        sampleIdEditor
    );

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
        semitoneLabel,
        semitoneEditor
    );

    addRow(
        pitchArea,
        fineTuneLabel,
        fineTuneEditor
    );

    // ========================================
    // VELOCITY CARD
    // ========================================

    auto velocityArea =
        velocityCardBounds.reduced(
            cardPadding
        );

    velocitySectionLabel.setBounds(
        velocityArea.removeFromTop(
            sectionHeight
        )
    );

    velocityRangeBar.setBounds(
        velocityArea.removeFromTop(45)
    );

    addRow(
        velocityArea,
        lowVelLabel,
        lowVelEditor
    );

    addRow(
        velocityArea,
        highVelLabel,
        highVelEditor
    );

    // ========================================
    // OUTPUT CARD
    // ========================================

    auto outputArea =
        outputCardBounds.reduced(
            cardPadding
        );

    outputSectionLabel.setBounds(
        outputArea.removeFromTop(
            sectionHeight
        )
    );

    addRow(
        outputArea,
        loudnessLabel,
        loudnessEditor
    );

    addRow(
        outputArea,
        panLabel,
        panEditor
    );

    addRow(
        outputArea,
        filterFreqLabel,
        filterFreqEditor
    );

    addRow(
        outputArea,
        playModeLabel,
        playModeCombo
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



    sampleCombo.addItem(
        "--- None ---",
        noneSampleComboId
    );

    for (const auto& [id, name] : residentSamples)
    {
        const int comboId = id + 1;



        sampleCombo.addItem(
            name,
            comboId
        );
    }
}