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

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadJson();
    void onLoadAsc();
    void onLoadDbc();

private:
    Ui::MainWindow *ui;
    DbcJsonParser parser;
    QLabel *labelStatus;
    void onSignalSelected(const QModelIndex &index);
private:
    QStandardItemModel *tableModel;
    bool convertDbcToJson(const QString &dbcPath, QString &jsonOutPath);
};

#endif
