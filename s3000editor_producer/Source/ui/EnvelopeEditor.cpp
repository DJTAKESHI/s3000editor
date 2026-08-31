#include "EnvelopeEditor.h"

EnvelopeEditor::EnvelopeEditor()
{
}

void EnvelopeEditor::setEnvelope(
    int attack,
    int decay,
    int sustain,
    int release)
{
    attackValue = juce::jlimit(0, 100, attack);
    decayValue = juce::jlimit(0, 100, decay);
    sustainValue = juce::jlimit(0, 100, sustain);
    releaseValue = juce::jlimit(0, 100, release);

    DBG(
        "ENV1 A=" + juce::String(attackValue)
        + " D=" + juce::String(decayValue)
        + " S=" + juce::String(sustainValue)
        + " R=" + juce::String(releaseValue)
    );

    repaint();
}

void EnvelopeEditor::paint(
    juce::Graphics& g)
{
    auto bounds =
        getLocalBounds()
        .toFloat()
        .reduced(12.0f);

    g.setColour(
        juce::Colours::darkgrey
    );

    g.fillRoundedRectangle(
        bounds,
        6.0f
    );

    bounds.reduce(14.0f, 14.0f);

    const float left =
        bounds.getX();

    const float right =
        bounds.getRight();

    const float top =
        bounds.getY();

    const float bottom =
        bounds.getBottom();

    const float width =
        bounds.getWidth();

    // 各ステージに最大幅を割り当てる
    const float attackWidth =
        width * 0.25f *
        (attackValue / 100.0f);

    const float decayWidth =
        width * 0.25f *
        (decayValue / 100.0f);

    const float releaseWidth =
        width * 0.25f *
        (releaseValue / 100.0f);

    const float x0 = left;

    const float x1 =
        x0 + attackWidth;

    const float x2 =
        x1 + decayWidth;

    const float x4 =
        right;

    const float x3 =
        juce::jmax(
            x2,
            x4 - releaseWidth
        );

    const float sustainY =
        bottom
        - (bounds.getHeight()
            * sustainValue / 100.0f);

    g.setColour(
        juce::Colours::white
    );

    // Attack
    g.drawLine(
        x0,
        bottom,
        x1,
        top,
        2.0f
    );

    // Decay
    g.drawLine(
        x1,
        top,
        x2,
        sustainY,
        2.0f
    );

    // Sustain
    g.drawLine(
        x2,
        sustainY,
        x3,
        sustainY,
        2.0f
    );

    // Release
    g.drawLine(
        x3,
        sustainY,
        x4,
        bottom,
        2.0f
    );

    const float pointRadius = 4.0f;

    g.fillEllipse(
        x1 - pointRadius,
        top - pointRadius,
        pointRadius * 2.0f,
        pointRadius * 2.0f
    );

    g.fillEllipse(
        x2 - pointRadius,
        sustainY - pointRadius,
        pointRadius * 2.0f,
        pointRadius * 2.0f
    );

    g.fillEllipse(
        x3 - pointRadius,
        sustainY - pointRadius,
        pointRadius * 2.0f,
        pointRadius * 2.0f
    );

    g.fillEllipse(
        x4 - pointRadius,
        bottom - pointRadius,
        pointRadius * 2.0f,
        pointRadius * 2.0f
    );

}

juce::Rectangle<float>
EnvelopeEditor::getGraphBounds() const
{
    auto bounds =
        getLocalBounds()
        .toFloat()
        .reduced(12.0f);

    bounds.reduce(14.0f, 14.0f);

    return bounds;
}

juce::Point<float>
EnvelopeEditor::getAttackPoint() const
{
    const auto bounds =
        getGraphBounds();

    const float attackWidth =
        bounds.getWidth()
        * 0.25f
        * (attackValue / 100.0f);

    return {
        bounds.getX() + attackWidth,
        bounds.getY()
    };
}

juce::Point<float>
EnvelopeEditor::getDecayPoint() const
{
    const auto bounds = getGraphBounds();

    const float attackWidth =
        bounds.getWidth()
        * 0.25f
        * (attackValue / 100.0f);

    const float decayWidth =
        bounds.getWidth()
        * 0.25f
        * (decayValue / 100.0f);

    const float sustainY =
        bounds.getBottom()
        - bounds.getHeight()
        * (sustainValue / 100.0f);

    return {
        bounds.getX() + attackWidth + decayWidth,
        sustainY
    };
}

juce::Point<float>
EnvelopeEditor::getSustainPoint() const
{
    const auto bounds = getGraphBounds();

    const auto decayPoint =
        getDecayPoint();

    const float releaseWidth =
        bounds.getWidth()
        * 0.25f
        * (releaseValue / 100.0f);

    const float x =
        juce::jmax(
            decayPoint.x,
            bounds.getRight() - releaseWidth
        );

    return {
        x,
        decayPoint.y
    };
}

juce::Point<float>
EnvelopeEditor::getReleasePoint() const
{
    const auto bounds = getGraphBounds();

    return {
        bounds.getRight(),
        bounds.getBottom()
    };
}

void EnvelopeEditor::notifyEnvelopeChanged()
{
    if (onEnvelopeChanged)
    {
        onEnvelopeChanged(
            attackValue,
            decayValue,
            sustainValue,
            releaseValue
        );
    }
}

void EnvelopeEditor::mouseDown(
    const juce::MouseEvent& event)
{
    const auto mouse = event.position;

    constexpr float hitRadius = 12.0f;

    if (mouse.getDistanceFrom(
        getAttackPoint()) <= hitRadius)
    {
        dragPoint = DragPoint::attack;
    }
    else if (mouse.getDistanceFrom(
        getDecayPoint()) <= hitRadius)
    {
        dragPoint = DragPoint::decay;
    }
    else if (mouse.getDistanceFrom(
        getSustainPoint()) <= hitRadius)
    {
        dragPoint = DragPoint::sustain;
    }
    else if (mouse.getDistanceFrom(
        getReleasePoint()) <= hitRadius)
    {
        dragPoint = DragPoint::release;
    }
}

void EnvelopeEditor::mouseDrag(
    const juce::MouseEvent& event)
{
    const auto bounds = getGraphBounds();

    if (dragPoint == DragPoint::none)
        return;

    if (dragPoint == DragPoint::attack)
    {
        const float maxWidth =
            bounds.getWidth() * 0.25f;

        const float x =
            juce::jlimit(
                bounds.getX(),
                bounds.getX() + maxWidth,
                event.position.x
            );

        attackValue =
            juce::jlimit(
                0,
                100,
                juce::roundToInt(
                    (x - bounds.getX())
                    / maxWidth * 100.0f
                )
            );
    }
    else if (dragPoint == DragPoint::decay)
    {
        const auto attackPoint =
            getAttackPoint();

        const float maxWidth =
            bounds.getWidth() * 0.25f;

        const float x =
            juce::jlimit(
                attackPoint.x,
                attackPoint.x + maxWidth,
                event.position.x
            );

        decayValue =
            juce::jlimit(
                0,
                100,
                juce::roundToInt(
                    (x - attackPoint.x)
                    / maxWidth * 100.0f
                )
            );
    }
    else if (dragPoint == DragPoint::sustain)
    {
        const float y =
            juce::jlimit(
                bounds.getY(),
                bounds.getBottom(),
                event.position.y
            );

        sustainValue =
            juce::jlimit(
                0,
                100,
                juce::roundToInt(
                    (bounds.getBottom() - y)
                    / bounds.getHeight()
                    * 100.0f
                )
            );
    }
    else if (dragPoint == DragPoint::release)
    {
        const float maxWidth =
            bounds.getWidth() * 0.25f;

        const float x =
            juce::jlimit(
                bounds.getRight() - maxWidth,
                bounds.getRight(),
                event.position.x
            );

        releaseValue =
            juce::jlimit(
                0,
                100,
                juce::roundToInt(
                    (bounds.getRight() - x)
                    / maxWidth * 100.0f
                )
            );
    }

    notifyEnvelopeChanged();
    repaint();
}

void EnvelopeEditor::mouseUp(
    const juce::MouseEvent&)
{
    if (dragPoint == DragPoint::none)
        return;

    dragPoint = DragPoint::none;

    if (onEditFinished)
        onEditFinished();
}


void EnvelopeEditor::resized()
{
}