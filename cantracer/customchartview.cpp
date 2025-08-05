#include "customchartview.h"

CustomChartView::CustomChartView(QWidget *parent)
    : QChartView(parent)
{
    setRubberBand(QChartView::RectangleRubberBand); // permet le zoom par sélection
}

void CustomChartView::enableCursors(bool enable)
{
    cursorsEnabled = enable;

    if (enable && (!cursor1 || !cursor2)) {
        createCursors();
    }

    if (cursor1) cursor1->setVisible(enable);
    if (cursor2) cursor2->setVisible(enable);
}

void CustomChartView::createCursors()
{
    if (!cursor1) {
        cursor1 = new QGraphicsLineItem();
        cursor1->setPen(QPen(Qt::red, 1));
        chart()->scene()->addItem(cursor1);
        cursor1->setLine(chart()->plotArea().left(), 0, chart()->plotArea().left(), chart()->plotArea().bottom());
    }

    if (!cursor2) {
        cursor2 = new QGraphicsLineItem();
        cursor2->setPen(QPen(Qt::blue, 1));
        chart()->scene()->addItem(cursor2);
        cursor2->setLine(chart()->plotArea().right(), 0, chart()->plotArea().right(), chart()->plotArea().bottom());
    }
}

void CustomChartView::mousePressEvent(QMouseEvent *event)
{
    if (cursorsEnabled) {
        QPointF pos = chart()->mapToValue(event->pos());

        // Choisir le curseur le plus proche
        qreal cursor1X = chart()->mapToPosition(QPointF(cursor1->line().x1(), 0)).x();
        qreal cursor2X = chart()->mapToPosition(QPointF(cursor2->line().x1(), 0)).x();

        qreal distance1 = std::abs(cursor1X - event->pos().x());
        qreal distance2 = std::abs(cursor2X - event->pos().x());

        activeCursor = (distance1 < distance2) ? cursor1 : cursor2;
    }

    QChartView::mousePressEvent(event); // permet le zoom
}

void CustomChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (cursorsEnabled && activeCursor) {
        QPointF value = chart()->mapToValue(event->pos());
        QLineF newLine(value.x(), chart()->plotArea().top(), value.x(), chart()->plotArea().bottom());
        activeCursor->setLine(chart()->mapToPosition(QPointF(value.x(), 0)).x(), chart()->plotArea().top(),
                              chart()->mapToPosition(QPointF(value.x(), 0)).x(), chart()->plotArea().bottom());
    }

    QChartView::mouseMoveEvent(event); // permet la sélection
}
