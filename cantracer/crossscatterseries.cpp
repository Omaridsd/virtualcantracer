#include "crossscatterseries.h"
#include <QPainter>

CrossScatterSeries::CrossScatterSeries(QObject *parent)
    : QtCharts::QScatterSeries(parent)
{
}

void CrossScatterSeries::drawMarker(QPainter *painter, const QPointF &point)
{
    const int size = 8; // taille de la croix
    painter->save();
    painter->setPen(QPen(color(), 2));

    // Dessiner une croix centrée sur "point"
    painter->drawLine(QPointF(point.x() - size/2, point.y() - size/2),
                      QPointF(point.x() + size/2, point.y() + size/2));
    painter->drawLine(QPointF(point.x() - size/2, point.y() + size/2),
                      QPointF(point.x() + size/2, point.y() - size/2));

    painter->restore();
}
