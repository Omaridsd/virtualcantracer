#ifndef CURSORMANAGER_H
#define CURSORMANAGER_H

#include <QObject>
#include <QGraphicsLineItem>
#include <QtCharts/QChart>
#include <QMouseEvent>
namespace QtCharts {
class QChart;
}

class CursorManager : public QObject
{
    Q_OBJECT

public:
    explicit CursorManager(QChart* chart, QObject* parent = nullptr);

    void installOnChartView(QObject* chartView);
    void setCursorPositions(qreal x1, qreal x2);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    QtCharts::QChart* chart;
    QGraphicsLineItem* cursorLine1;
    QGraphicsLineItem* cursorLine2;
};

#endif // CURSORMANAGER_H
