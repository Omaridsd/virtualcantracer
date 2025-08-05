#include "customchartview.h"
#include <QtCharts/QChart>
#include <QDebug>

CustomChartView::CustomChartView(QWidget *parent)
    : QChartView(parent)
{}

void CustomChartView::setCursors(QGraphicsLineItem* c1, QGraphicsLineItem* c2) {
    cursor1 = c1;
    cursor2 = c2;
}

void CustomChartView::enableCursors(bool enable) {
    cursorsEnabled = enable;
}

void CustomChartView::mousePressEvent(QMouseEvent *event) {
    if (cursorsEnabled) {
        QPointF mousePos = event->pos();
        QPointF valuePos = this->chart()->mapToValue(mousePos);
        qreal xValue = valuePos.x();

        qreal x1 = chart()->mapToValue(cursor1->line().p1()).x();
        qreal x2 = chart()->mapToValue(cursor2->line().p1()).x();

        QGraphicsLineItem* targetCursor = (std::abs(xValue - x1) < std::abs(xValue - x2)) ? cursor1 : cursor2;

        QPointF scenePos = this->chart()->mapToPosition(QPointF(xValue, 0));
        QLineF newLine(scenePos.x(), this->chart()->plotArea().top(),
                       scenePos.x(), this->chart()->plotArea().bottom());
        targetCursor->setLine(newLine);

        // Émettre le signal pour informer du déplacement
        int cursorId = (targetCursor == cursor1) ? 1 : 2;
        emit cursorMoved(scenePos.x(), cursorId);
    }
    QChartView::mousePressEvent(event);
}
