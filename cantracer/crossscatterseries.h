#ifndef CROSSSCATTERSERIES_H
#define CROSSSCATTERSERIES_H

#include <QtCharts/QScatterSeries>

class CrossScatterSeries : public QtCharts::QScatterSeries {
    Q_OBJECT
public:
    explicit CrossScatterSeries(QObject *parent = nullptr);

protected:
    void drawMarker(QPainter *painter, const QPointF &point) override;
};

#endif // CROSSSCATTERSERIES_H
