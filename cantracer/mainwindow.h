#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QStandardItemModel>
#include <QMainWindow>
#include "dbc_json_parser.h"
#include <QLabel>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QGraphicsLineItem>
#include <QMouseEvent>
#include <QListWidget>
#include "signal_model.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QMap>
#include <QtCharts/QDateTimeAxis>
#include "can_decoder.h"  // ou ton fichier qui contient la classe CanDecoder
#include "can_frame.h"
#include <QDateTime>
#include "asc_reader.h"
#include <QTreeWidgetItem>
#include <QPair>
#include "customchartview.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
struct DataPoint {
    double timestamp;
    double value;
};
struct CursorLines {
    QGraphicsLineItem* line1;
    QGraphicsLineItem* line2;
};



QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
    Q_OBJECT
    QList<QChartView*> checkedCharts;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadJson();
    void onLoadAsc();
    void onLoadDbc();
    void onChangeGraphColor();

private:
    void setCursorPos1(double newPos);
    void setCursorPos2(double newPos);
    bool placingFirstCursor = true;
    QMap<QChart*, QGraphicsLineItem*> cursorLine1Map;
    QMap<QChart*, QGraphicsLineItem*> cursorLine2Map;

    Ui::MainWindow *ui;
    AscReader* ascReader;  // Pointeur vers ton objet AscReader
    void hideSignalGraph(const QString& signalName);
    QMap<QChartView*, QPair<QGraphicsLineItem*, QGraphicsLineItem*>> cursorMap;

    QMap<QChart*, CursorLines> cursorLinesMap;
    double cursorPos1 = 0.5;
    double cursorPos2 = 1.0;
    void updateCursorLines();
    QList<Message> messages;  // ✅ Stocke les messages DBC
    QList<CanFrame> frames;   // ✅ Stocke les trames ASC
    QDateTime baseDateTime;
private:
    qreal currentCursorX = 0.0;     // Position du premier curseur
    qreal secondCursorX = 0.0;      // Position du deuxième curseur
    void addCursorLinesToChart(QChart* chart);
    DbcJsonParser parser;
    QLabel *labelStatus;
    void onSignalSelected(const QModelIndex &index);
    QLabel *labelCoord = nullptr;
    QChart *chart;
    QChartView *chartView;
    QGraphicsLineItem *cursorLine = nullptr;
    QChartView* selectedChartView = nullptr;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void resetZoomForAllGraphs();
    QDateTime startDateTime;  // Heure de début extraite de l'en-tête ASC
    void onCursorLineMoved(qreal x);

private:
    void addCursorLinesToChart(QChartView* view);

    QSet<QString> loadedSignals;
    QList<QChartView*> graphViews;  // Déclaration de la liste de graphiques
    void onSignalListUpdated(QListWidgetItem* /*item*/);
    QDateTime ascStartDate;
    void populateTreeFromMessages(const QList<Message>& messages);
    void onTreeItemChanged(QTreeWidgetItem* item, int column);
    void onSignalListUpdated();
    void on_btnActivateCursors_clicked();
    void disableRubberBand();
    void enableRubberBand();
    void clearAllData();
    QStandardItemModel *tableModel;
    bool convertDbcToJson(const QString &dbcPath, QString &jsonOutPath);
    void addSeriesForSignal(const QString &signalName);
    void removeSeriesForSignal(const QString &signalName);
    void updateSignalListWidget();
    void showSignalGraph(const QString &signalName);  // Affiche un graphique pour un signal
    void onMultipleSignalsSelected(const QStringList &selectedSignals);  // Affiche plusieurs graphiques
    void removeSignalGraph(const QString& signalName);
    QScrollArea *scrollAreaGraphs;
    QWidget *scrollContentWidget;
    QVBoxLayout *graphsLayout;
    void showSignalGraphStacked(const QString &signalName);
    QMap<QString, QVector<DataPoint>> signalMap;
    QMap<QString, QLineSeries*> signalSeriesMap;
    QMap<QString, QChartView*> signalChartViewsMap;
    void updateSignalTreeWidget();
    QMap<QChartView*, QPair<QGraphicsLineItem*, QGraphicsLineItem*>> cursorLines;
    QGraphicsLineItem* cursorLine1 = nullptr;
    QGraphicsLineItem* cursorLine2 = nullptr;
    bool cursorsEnabled = false;
    void onCursorMoved(qreal xPos, int cursorId, CustomChartView* sourceChartView);
private:
    QLabel* labelDeltaT = nullptr;

private slots:
    void onSignalCheckChanged(QListWidgetItem* item);

};

#endif
