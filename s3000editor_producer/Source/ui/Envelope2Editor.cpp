#include "Envelope2Editor.h"

Envelope2Editor::Envelope2Editor()
{
}

void Envelope2Editor::setEnvelope(
    int r1, int l1,
    int r2, int l2,
    int r3, int l3,
    int r4, int l4)
{
    r1Value = juce::jlimit(0, 100, r1);
    l1Value = juce::jlimit(0, 100, l1);

    r2Value = juce::jlimit(0, 100, r2);
    l2Value = juce::jlimit(0, 100, l2);

    r3Value = juce::jlimit(0, 100, r3);
    l3Value = juce::jlimit(0, 100, l3);

    r4Value = juce::jlimit(0, 100, r4);
    l4Value = juce::jlimit(0, 100, l4);

    repaint();

}

void Envelope2Editor::paint(
    juce::Graphics& g)
{
    auto bounds =
        getLocalBounds()
        .toFloat()
        .reduced(12.0f);

    g.setColour(juce::Colours::darkgrey);
    g.fillRoundedRectangle(bounds, 6.0f);

    bounds.reduce(14.0f, 14.0f);

    const float left =
        bounds.getX();

    const float bottom =
        bounds.getBottom();

    const float width =
        bounds.getWidth();

    const float height =
        bounds.getHeight();

    const float stageWidth =
        width * 0.25f;

    const float x0 = left;

    const float x1 =
        x0
        + stageWidth
        * (r1Value / 100.0f);

    const float x2 =
        x1
        + stageWidth
        * (r2Value / 100.0f);

    const float x3 =
        x2
        + stageWidth
        * (r3Value / 100.0f);

    const float x4 =
        juce::jmin(
            bounds.getRight(),
            x3
            + stageWidth
            * (r4Value / 100.0f)
        );

    const float y0 = bottom;

    const float y1 =
        bottom
        - height
        * (l1Value / 100.0f);

    const float y2 =
        bottom
        - height
        * (l2Value / 100.0f);

    const float y3 =
        bottom
        - height
        * (l3Value / 100.0f);

    const float y4 =
        bottom
        - height
        * (l4Value / 100.0f);

    g.setColour(juce::Colours::white);

    g.drawLine(
        x0, y0,
        x1, y1,
        2.0f);

    g.drawLine(
        x1, y1,
        x2, y2,
        2.0f);

    g.drawLine(
        x2, y2,
        x3, y3,
        2.0f);

    g.drawLine(
        x3, y3,
        x4, y4,
        2.0f);

    constexpr float radius = 4.0f;

    g.fillEllipse(
        x1 - radius,
        y1 - radius,
        radius * 2.0f,
        radius * 2.0f);

    g.fillEllipse(
        x2 - radius,
        y2 - radius,
        radius * 2.0f,
        radius * 2.0f);

    g.fillEllipse(
        x3 - radius,
        y3 - radius,
        radius * 2.0f,
        radius * 2.0f);

    g.fillEllipse(
        x4 - radius,
        y4 - radius,
        radius * 2.0f,
        radius * 2.0f);
}

juce::Rectangle<float>
Envelope2Editor::getGraphBounds() const
{
    auto bounds =
        getLocalBounds()
        .toFloat()
        .reduced(12.0f);

    bounds.reduce(14.0f, 14.0f);

    return bounds;
}




juce::Point<float>
Envelope2Editor::getPoint1() const
{
    const auto bounds =
        getGraphBounds();

    const float stageWidth =
        bounds.getWidth() * 0.25f;

    const float x =
        bounds.getX()
        + stageWidth
        * (r1Value / 100.0f);

    const float y =
        bounds.getBottom()
        - bounds.getHeight()
        * (l1Value / 100.0f);

    return { x, y };
}

void Envelope2Editor::notifyEnvelopeChanged()
{
    if (onEnvelopeChanged)
    {
        onEnvelopeChanged(
            r1Value, l1Value,
            r2Value, l2Value,
            r3Value, l3Value,
            r4Value, l4Value
        );
    }
}


void Envelope2Editor::mouseDown(
    const juce::MouseEvent& event)
{
    constexpr float hitRadius = 12.0f;

    const auto mouse = event.position;

    if (mouse.getDistanceFrom(getPoint1()) <= hitRadius)
        dragPoint = DragPoint::point1;

    else if (mouse.getDistanceFrom(getPoint2()) <= hitRadius)
        dragPoint = DragPoint::point2;

    else if (mouse.getDistanceFrom(getPoint3()) <= hitRadius)
        dragPoint = DragPoint::point3;

    else if (mouse.getDistanceFrom(getPoint4()) <= hitRadius)
        dragPoint = DragPoint::point4;
}

void Envelope2Editor::mouseDrag(
    const juce::MouseEvent& event)
{
    if (dragPoint == DragPoint::none)
        return;

    const auto bounds = getGraphBounds();

    const float stageWidth =
        bounds.getWidth() * 0.25f;

    auto levelFromY =
        [&]()
        {
            const float y =
                juce::jlimit(
                    bounds.getY(),
                    bounds.getBottom(),
                    event.position.y
                );

            return juce::jlimit(
                0,
                100,
                juce::roundToInt(
                    (bounds.getBottom() - y)
                    / bounds.getHeight()
                    * 100.0f
                )
            );
        };

    if (dragPoint == DragPoint::point1)
    {
        const float x =
            juce::jlimit(
                bounds.getX(),
                bounds.getX() + stageWidth,
                event.position.x
            );

        r1Value =
            juce::jlimit(
                0,
                100,
                juce::roundToInt(
                    (x - bounds.getX())
                    / stageWidth * 100.0f
                )
            );

        l1Value = levelFromY();
    }
    else if (dragPoint == DragPoint::point2)
    {
        const auto p1 = getPoint1();

        const float x =
            juce::jlimit(
                p1.x,
                p1.x + stageWidth,
                event.position.x
            );

        r2Value =
            juce::jlimit(
                0,
                100,
                juce::roundToInt(
                    (x - p1.x)
                    / stageWidth * 100.0f
                )
            );

        l2Value = levelFromY();
    }
    else if (dragPoint == DragPoint::point3)
    {
        const auto p2 = getPoint2();

        const float x =
            juce::jlimit(
                p2.x,
                p2.x + stageWidth,
                event.position.x
            );

        r3Value =
            juce::jlimit(
                0,
                100,
                juce::roundToInt(
                    (x - p2.x)
                    / stageWidth * 100.0f
                )
            );

        l3Value = levelFromY();
    }
    else if (dragPoint == DragPoint::point4)
    {
        const auto p3 = getPoint3();

        const float maxX =
            juce::jmin(
                bounds.getRight(),
                p3.x + stageWidth
            );

        const float x =
            juce::jlimit(
                p3.x,
                maxX,
                event.position.x
            );

        const float usableWidth =
            juce::jmax(
                1.0f,
                maxX - p3.x
            );

        r4Value =
            juce::jlimit(
                0,
                100,
                juce::roundToInt(
                    (x - p3.x)
                    / usableWidth * 100.0f
                )
            );

        l4Value = levelFromY();
    }

    notifyEnvelopeChanged();
    repaint();
}

void Envelope2Editor::mouseUp(
    const juce::MouseEvent&)
{
    if (dragPoint == DragPoint::none)
        return;

    dragPoint = DragPoint::none;

    if (onEditFinished)
        onEditFinished();
}

juce::Point<float>
Envelope2Editor::getPoint2() const
{
    const auto bounds = getGraphBounds();
    const auto p1 = getPoint1();

    const float stageWidth =
        bounds.getWidth() * 0.25f;

    const float x =
        p1.x
        + stageWidth
        * (r2Value / 100.0f);

    const float y =
        bounds.getBottom()
        - bounds.getHeight()
        * (l2Value / 100.0f);

    return { x, y };
}

juce::Point<float>
Envelope2Editor::getPoint3() const
{
    const auto bounds = getGraphBounds();
    const auto p2 = getPoint2();

    const float stageWidth =
        bounds.getWidth() * 0.25f;

    const float x =
        p2.x
        + stageWidth
        * (r3Value / 100.0f);

    const float y =
        bounds.getBottom()
        - bounds.getHeight()
        * (l3Value / 100.0f);

    return { x, y };
}

juce::Point<float>
Envelope2Editor::getPoint4() const
{
    const auto bounds = getGraphBounds();
    const auto p3 = getPoint3();

    const float stageWidth =
        bounds.getWidth() * 0.25f;

    const float x =
        juce::jmin(
            bounds.getRight(),
            p3.x
            + stageWidth
            * (r4Value / 100.0f)
        );

    const float y =
        bounds.getBottom()
        - bounds.getHeight()
        * (l4Value / 100.0f);

    return { x, y };
}


