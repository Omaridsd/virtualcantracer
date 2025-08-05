#ifndef CUSTOMCHARTVIEW_H
#define CUSTOMCHARTVIEW_H

#include <QtCharts/QChartView>
#include <QGraphicsLineItem>
#include <QMouseEvent>


    class CustomChartView : public QChartView
{
    Q_OBJECT

public:
    QGraphicsLineItem* getCursor1() const { return cursor1; }
    QGraphicsLineItem* getCursor2() const { return cursor2; }

public:
    explicit CustomChartView(QWidget *parent = nullptr);
    void setCursors(QGraphicsLineItem* c1, QGraphicsLineItem* c2);
    void enableCursors(bool enable);
signals:
    void cursorMoved(qreal xPos, int cursorId);
protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QGraphicsLineItem* cursor1 = nullptr;
    QGraphicsLineItem* cursor2 = nullptr;
    bool cursorsEnabled = false;
};

#endif // CUSTOMCHARTVIEW_H
