#ifndef CUSTOMCHARTVIEW_H
#define CUSTOMCHARTVIEW_H
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QScatterSeries>

#include <QGraphicsLineItem>
#include <QMouseEvent>



class CustomChartView : public QChartView {
    Q_OBJECT

public:
    explicit CustomChartView(QWidget *parent = nullptr);
    void enableCursors(bool enable);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    bool cursorsEnabled = false;
    QGraphicsLineItem* cursor1 = nullptr;
    QGraphicsLineItem* cursor2 = nullptr;
    QGraphicsLineItem* activeCursor = nullptr;

    void createCursors();
};

#endif // CUSTOMCHARTVIEW_H
