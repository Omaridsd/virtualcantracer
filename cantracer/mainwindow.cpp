#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include "dbc_json_parser.h"
#include "asc_reader.h"
#include "signal_model.h"
#include "can_decoder.h"
#include <QProcess>
#include <QStandardPaths>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include "signal_selection_dialog.h"
#include <QToolTip>
#include <QtCharts/QDateTimeAxis>
#include <QColorDialog>
#include <QtCharts/QScatterSeries>
#include <QTimer>
#include "movablelineitem.h"
#include "customchartview.h"

// Déclaration de la fonction de décodage
double decodeSignal(const QByteArray &data, const CanSignal &sig);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionChangeGraphColor, &QAction::triggered, this, &MainWindow::onChangeGraphColor);
    //connect(ui->listSignals, &QListWidget::itemChanged,
            //this, &MainWindow::onSignalListUpdated);

    // Récupérer scrollArea et scrollContentWidget depuis le .ui
    scrollContentWidget = ui->scrollAreaGraphs->widget();
    connect(ui->treeSignals, &QTreeWidget::itemChanged, this, &MainWindow::onTreeItemChanged);

    // Créer ou récupérer le layout vertical pour les graphes
    graphsLayout = new QVBoxLayout(scrollContentWidget);
    graphsLayout->setContentsMargins(0, 0, 0, 0);
    graphsLayout->setSpacing(0);  // ou 0 si tu veux empiler sans aucun espace

    scrollContentWidget->setLayout(graphsLayout);
    //connect(ui->listSignals, &QListWidget::itemChanged, this, &MainWindow::onSignalCheckChanged);


    scrollAreaGraphs = ui->scrollAreaGraphs;
    scrollContentWidget = ui->scrollContentWidget;

    // Récupère le layout vertical de scrollContentWidget
    graphsLayout = qobject_cast<QVBoxLayout*>(scrollContentWidget->layout());

    connect(ui->actionResetZoom, &QAction::triggered, this, &MainWindow::resetZoomForAllGraphs);

    connect(ui->actionChargerASC, &QAction::triggered, this, &MainWindow::onLoadAsc);
    connect(ui->actionChargerDBC, &QAction::triggered, this, &MainWindow::onLoadDbc);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onLoadJson()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Ouvrir fichier JSON", "", "Fichiers JSON (*.json)");

    if (!filePath.isEmpty()) {
        bool ok = parser.loadFromFile(filePath);

        if (ok) {
            ui->labelStatus->setText(" Fichier JSON chargé avec succès : " + filePath);

            // Construire l'arbre avec messages et signaux
            populateTreeFromMessages(parser.getMessages());

        } else {
            ui->labelStatus->setText("Erreur lors du chargement du fichier JSON.");
        }
    } else {
        ui->labelStatus->setText("ℹ️ Aucun fichier sélectionné.");
    }
}

void MainWindow::onLoadAsc()
{
    clearAllData();
    QString filePath = QFileDialog::getOpenFileName(this, "Ouvrir fichier ASC", "", "Fichiers ASC (*.asc)");
    if (filePath.isEmpty()) return;

    AscReader reader;
    QList<CanFrame> frames = reader.readAscFile(filePath);

    qDebug() << "✅ Fichier ASC chargé. Nombre de trames :" << frames.size();

    for (const CanFrame &frame : frames) {
        qDebug() << "ID:" << QString::number(frame.id, 16).toUpper()
        << " Data:" << frame.data.toHex(' ').toUpper();
    }

    for (const CanFrame &frame : frames) {
        const Message *msg = parser.getMessageById(frame.id);

        if (msg) {
            qDebug() << "Message : " << msg->name;
            for (const CanSignal &sig : msg->signalss) {
                double value = decodeSignal(frame.data, sig);
                signalMap[sig.name].append({frame.timestamp, value});

                QList<QStandardItem*> row;
                row << new QStandardItem(QString::number(frame.timestamp))                     // Timestamp
                    << new QStandardItem(QString("0x%1").arg(frame.id, 0, 16).toUpper())       // ID
                    << new QStandardItem(msg->name)                                            // Message name
                    << new QStandardItem(sig.name)                                             // Signal name
                    << new QStandardItem(QString::number(value))                               // Signal value
                    << new QStandardItem(sig.unit);                                            // Unit

                qDebug() << "  Signal : " << sig.name << " = " << value << sig.unit;
            }
        } else {
            qDebug() << "Message inconnu, ID: " << QString::number(frame.id, 16).toUpper();
        }
    }
}
bool MainWindow::convertDbcToJson(const QString &dbcPath, QString &jsonOutPath) {
    jsonOutPath = QFileInfo(dbcPath).absolutePath() + "/converted.json";

    QProcess process;

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp(); // depuis build/debug
    dir.cdUp(); // projet
    dir.cd("python"); // ton dossier contenant les scripts Python

    QString fixScriptPath = dir.filePath("fix_dbc_names.py");
    QString converterScriptPath = dir.filePath("dbc_to_json.py");

    QString pythonExe = "C:/Users/DELL/AppData/Local/Programs/Thonny/python.exe";
    qDebug() << "✅ Python utilisé par Qt:" << pythonExe;

    if (pythonExe.isEmpty()) {
        qWarning() << "❌ Python non trouvé dans PATH.";
        return false;
    }

    // 1️⃣ Étape 1 : corriger le fichier DBC original
    QString fixedDbcPath = QFileInfo(dbcPath).absolutePath() + "/fixed.dbc";
    process.start(pythonExe, QStringList() << fixScriptPath << dbcPath << fixedDbcPath);
    process.waitForFinished();

    QByteArray fixOut = process.readAllStandardOutput();
    QByteArray fixErr = process.readAllStandardError();

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        qWarning() << "❌ Échec correction DBC.";
        qWarning() << "stdout:" << fixOut;
        qWarning() << "stderr:" << fixErr;
        return false;
    }
    qDebug() << "✅ Correction réussie. Nouveau DBC : " << fixedDbcPath;

    // 2️⃣ Étape 2 : convertir le fichier corrigé
    process.start(pythonExe, QStringList() << converterScriptPath << fixedDbcPath << jsonOutPath);
    process.waitForFinished();

    QByteArray stdOut = process.readAllStandardOutput();
    QByteArray stdErr = process.readAllStandardError();

    if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0) {
        qDebug() << "✅ Conversion réussie :" << jsonOutPath;
        qDebug() << "stdout:" << stdOut;
        return true;
    } else {
        qWarning() << "❌ Erreur conversion DBC → JSON :";
        qWarning() << "stdout:" << stdOut;
        qWarning() << "stderr:" << stdErr;
        return false;
    }
}


void MainWindow::onLoadDbc()
{
    QString dbcPath = QFileDialog::getOpenFileName(this, "Ouvrir fichier DBC", "", "Fichiers DBC (*.dbc)");
    if (dbcPath.isEmpty()) {
        ui->labelStatus->setText("ℹ️ Aucun fichier sélectionné.");
        return;
    }

    QString jsonPath;
    if (convertDbcToJson(dbcPath, jsonPath)) {
        if (parser.loadFromFile(jsonPath)) {
            ui->labelStatus->setText("✅ Fichier DBC converti et chargé avec succès !");
            updateSignalTreeWidget();
        } else {
            ui->labelStatus->setText("❌ Conversion DBC réussie mais chargement JSON échoué !");
        }
    } else {
        ui->labelStatus->setText("❌ Échec de la conversion DBC → JSON.");
    }
}
void MainWindow::onSignalSelected(const QModelIndex &index)
{
    if (!index.isValid()) return;

    QString signalName = tableModel->item(index.row(), 3)->text(); // colonne 3 : Signal
    QString messageName = tableModel->item(index.row(), 2)->text(); // colonne 2 : Message

    QLineSeries *series = new QLineSeries();
}
void MainWindow::updateSignalTreeWidget() {
    ui->treeSignals->clear();

    const QList<Message>& messages = parser.getMessages();

    for (const Message& msg : messages) {
        QString idText = QString("BO_ %1 %2").arg(msg.id).arg(msg.name);
        QTreeWidgetItem* messageItem = new QTreeWidgetItem(ui->treeSignals);
        messageItem->setText(0, idText);
        messageItem->setFlags(Qt::ItemIsEnabled); // message non cochable

        for (const CanSignal& sig : msg.signalss) {
            QTreeWidgetItem* signalItem = new QTreeWidgetItem(messageItem);
            signalItem->setText(0, sig.name);
            signalItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
            signalItem->setCheckState(0, Qt::Unchecked);
        }
    }

    ui->treeSignals->expandAll(); // Optionnel : pour tout dérouler
}

void MainWindow::onSignalListUpdated()
{
    bool foundChecked = false;

    // Parcourt récursivement tous les items de l'arbre
    std::function<void(QTreeWidgetItem*)> checkItem = [&](QTreeWidgetItem* item) {
        if (item->childCount() == 0 && item->checkState(0) == Qt::Checked) {
            foundChecked = true;
            return;
        }

        for (int i = 0; i < item->childCount(); ++i) {
            checkItem(item->child(i));
            if (foundChecked) return;
        }
    };

    for (int i = 0; i < ui->treeSignals->topLevelItemCount(); ++i) {
        checkItem(ui->treeSignals->topLevelItem(i));
        if (foundChecked) break;
    }

    if (foundChecked) {
        qDebug() << "✔️ Au moins un signal est coché dans le QTreeWidget.";
    } else {
        qDebug() << "❌ Aucun signal coché dans le QTreeWidget.";
    }
}

void MainWindow::showSignalGraph(const QString& signalName) {
    if (!signalMap.contains(signalName))
        return;

    const QList<DataPoint>& rawData = signalMap[signalName];

    QLineSeries* series = new QLineSeries();
    series->setName(signalName + " (courbe)");

    QScatterSeries* scatterSeries = new QScatterSeries();
    scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    scatterSeries->setMarkerSize(6.0);
    scatterSeries->setColor(Qt::red);
    scatterSeries->setName(signalName + " (points)");

    for (const DataPoint& point : rawData) {
        series->append(QPointF(point.timestamp, point.value));
        scatterSeries->append(QPointF(point.timestamp, point.value));
    }

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->addSeries(scatterSeries);
    chart->createDefaultAxes();

    // Utiliser CustomChartView au lieu de QChartView
    CustomChartView* chartView = new CustomChartView(this);
    chartView->setChart(chart);
    chartView->enableCursors(true);  // Active les deux curseurs

    // Synchronisation du zoom horizontal entre tous les graphiques
    QValueAxis* axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
    if (axisX) {
        connect(axisX, &QValueAxis::rangeChanged, this, [this, axisX](qreal min, qreal max) {
            for (int i = 0; i < graphsLayout->count(); ++i) {
                QChartView* otherView = qobject_cast<QChartView*>(graphsLayout->itemAt(i)->widget());
                if (otherView && otherView->chart()) {
                    QValueAxis* otherAxisX = qobject_cast<QValueAxis*>(otherView->chart()->axes(Qt::Horizontal).first());
                    if (otherAxisX && otherAxisX != axisX) {
                        otherAxisX->setRange(min, max);
                    }
                }
            }
        });
    }

    chartView->setMinimumHeight(150);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    chart->setTitle(signalName);

    // Ajout dans l'interface
    graphsLayout->addWidget(chartView);

    // Enregistre la vue dans la map
    signalChartViewsMap[signalName] = chartView;

    // Apparence
    chartView->setCursor(Qt::PointingHandCursor);
    chartView->installEventFilter(this);
    chart->setMargins(QMargins(0, 0, 0, 0));
    chart->legend()->hide();
    chart->setBackgroundRoundness(0);
    chart->setBackgroundVisible(false);
}


void MainWindow::removeSignalGraph(const QString& signalName) {
    for (int i = 0; i < graphsLayout->count(); ++i) {
        QChartView* view = qobject_cast<QChartView*>(graphsLayout->itemAt(i)->widget());
        if (view && view->chart()->title() == signalName) {
            QWidget* widget = graphsLayout->takeAt(i)->widget();
            widget->deleteLater();
            break;
        }
    }
}
void MainWindow::resetZoomForAllGraphs()
{
    for (int i = 0; i < graphsLayout->count(); ++i) {
        QChartView* view = qobject_cast<QChartView*>(graphsLayout->itemAt(i)->widget());
        if (view) {
            view->chart()->zoomReset();
        }
    }
}
void MainWindow::clearAllData()
{
    // Vider les données des signaux
    signalMap.clear();

    // Supprimer tous les graphiques du layout
    while (graphsLayout->count() > 0) {
        QLayoutItem* item = graphsLayout->takeAt(0);
        if (item) {
            QWidget* widget = item->widget();
            if (widget) {
                widget->deleteLater();
            }
            delete item;
        }
    }

    // Réinitialiser la liste des signaux cochés
    loadedSignals.clear();
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        if (mouseEvent->button() == Qt::LeftButton) {
            QChartView* clickedChart = qobject_cast<QChartView*>(obj);
            if (!clickedChart)
                return false;

            // Vérifie s’il y a au moins un signal coché
            bool foundChecked = false;
            QTreeWidget* tree = ui->treeSignals;
            for (int i = 0; i < tree->topLevelItemCount() && !foundChecked; ++i) {
                QTreeWidgetItem* msgItem = tree->topLevelItem(i);
                for (int j = 0; j < msgItem->childCount(); ++j) {
                    if (msgItem->child(j)->checkState(0) == Qt::Checked) {
                        foundChecked = true;
                        break;
                    }
                }
            }

            if (!foundChecked) {
                qDebug() << "⚠️ Aucun signal coché.";
                return false;
            }

            selectedChartView = clickedChart;
            QChart* chart = clickedChart->chart();

            // Convertir la position de la souris en coordonnées de l’axe X
            QPointF posInChart = chart->mapToValue(mouseEvent->pos(), chart->series().first());
            qreal xVal = posInChart.x();

            QValueAxis* axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());
            if (!axisY) return false;

            // Calculer les positions top et bottom dans la scène pour une ligne verticale à xVal
            QPointF topPoint = chart->mapToPosition(QPointF(xVal, axisY->max()));
            QPointF bottomPoint = chart->mapToPosition(QPointF(xVal, axisY->min()));

            QPointF sceneTop = clickedChart->mapToScene(topPoint.toPoint());
            QPointF sceneBottom = clickedChart->mapToScene(bottomPoint.toPoint());

            if (placingFirstCursor) {
                // Supprimer ancien curseur 1 si existe
                if (cursorLine1) {
                    cursorLine1->scene()->removeItem(cursorLine1);
                    delete cursorLine1;
                    cursorLine1 = nullptr;
                }

                cursorLine1 = new MovableLineItem(QLineF(sceneTop, sceneBottom));
                cursorLine1->setPen(QPen(Qt::red, 1, Qt::DashLine));
                chart->scene()->addItem(cursorLine1);
                connect(static_cast<MovableLineItem*>(cursorLine1), &MovableLineItem::positionChanged, this, &MainWindow::onCursorLineMoved);

                cursorLine1->setZValue(1000);
                qDebug() << "🔴 Curseur 1 placé à x =" << xVal;
            } else {
                // Supprimer ancien curseur 2 si existe
                if (cursorLine2) {
                    cursorLine2->scene()->removeItem(cursorLine2);
                    delete cursorLine2;
                    cursorLine2 = nullptr;
                }

                cursorLine2 = new MovableLineItem(QLineF(sceneTop, sceneBottom));
                cursorLine2->setPen(QPen(Qt::blue, 1, Qt::DashLine));
                chart->scene()->addItem(cursorLine2);
                connect(static_cast<MovableLineItem*>(cursorLine2), &MovableLineItem::positionChanged, this, &MainWindow::onCursorLineMoved);

                cursorLine2->setZValue(1000);
                qDebug() << "🔵 Curseur 2 placé à x =" << xVal;
            }

            placingFirstCursor = !placingFirstCursor; // alterne entre les deux lignes
        }
    }

    return QMainWindow::eventFilter(obj, event);
}


void MainWindow::onChangeGraphColor() {
    if (!selectedChartView) {
        qDebug() << "Aucun graphique sélectionné.";
        return;
    }

    QColor color = QColorDialog::getColor(Qt::white, this, "Choisir une couleur pour la courbe");

    if (!color.isValid()) return;

    QChart* chart = selectedChartView->chart();
    for (QAbstractSeries* s : chart->series()) {
        QLineSeries* line = qobject_cast<QLineSeries*>(s);
        if (line) line->setColor(color);
    }
}

void MainWindow::onSignalCheckChanged(QListWidgetItem* item)
{
    QString signalName = item->text();

    if (item->checkState() == Qt::Checked) {
        showSignalGraph(signalName);
    } else {
        removeSignalGraph(signalName);
    }
}
void MainWindow::populateTreeFromMessages(const QList<Message>& messages) {
    ui->treeSignals->clear();

    for (const Message& msg : messages) {
        QString idText = QString("BO_ %1 %2").arg(msg.id).arg(msg.name);
        QTreeWidgetItem* messageItem = new QTreeWidgetItem(ui->treeSignals);
        messageItem->setText(0, idText);
        messageItem->setFlags(Qt::ItemIsEnabled); // Pas cochable

        for (const CanSignal& sig : msg.signalss) {
            QTreeWidgetItem* signalItem = new QTreeWidgetItem(messageItem);
            signalItem->setText(0, sig.name);
            signalItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
            signalItem->setCheckState(0, Qt::Unchecked);
        }
    }

    // Ferme tous les items parents
    ui->treeSignals->collapseAll();
}


void MainWindow::onTreeItemChanged(QTreeWidgetItem* item, int column) {
    if (!item || item->childCount() > 0) return; // Ignore les items parents

    QString signalName = item->text(0);

    if (item->checkState(0) == Qt::Checked) {
        showSignalGraph(signalName); // Utilise ta fonction existante
    } else {
        hideSignalGraph(signalName); // Crée cette fonction si elle n’existe pas
    }
}

void MainWindow::hideSignalGraph(const QString& signalName) {
    if (!signalChartViewsMap.contains(signalName))
        return;

    QChartView* chartView = signalChartViewsMap[signalName];
    if (chartView) {
        graphsLayout->removeWidget(chartView);
        chartView->setParent(nullptr);
        delete chartView;
    }
    signalChartViewsMap.remove(signalName);
}

void MainWindow::addCursorLinesToChart(QChart* chart) {
    if (!chart || !chart->scene())
        return;

    QValueAxis* axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
    QValueAxis* axisY = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).first());

    if (!axisX || !axisY) return;

    qreal x1 = axisX->min() + (axisX->max() - axisX->min()) * 0.1;
    qreal x2 = axisX->min() + (axisX->max() - axisX->min()) * 0.3;

    QPointF top1 = chart->mapToPosition(QPointF(x1, axisY->max()));
    QPointF bottom1 = chart->mapToPosition(QPointF(x1, axisY->min()));

    QPointF top2 = chart->mapToPosition(QPointF(x2, axisY->max()));
    QPointF bottom2 = chart->mapToPosition(QPointF(x2, axisY->min()));

    // Supprimer anciennes lignes si elles existent
    if (cursorLine1Map.contains(chart)) {
        chart->scene()->removeItem(cursorLine1Map[chart]);
        delete cursorLine1Map[chart];
    }
    if (cursorLine2Map.contains(chart)) {
        chart->scene()->removeItem(cursorLine2Map[chart]);
        delete cursorLine2Map[chart];
    }

    QGraphicsLineItem* line1 = chart->scene()->addLine(QLineF(top1, bottom1), QPen(Qt::red, 1, Qt::DashLine));
    QGraphicsLineItem* line2 = chart->scene()->addLine(QLineF(top2, bottom2), QPen(Qt::blue, 1, Qt::DashLine));

    line1->setZValue(1000);
    line2->setZValue(1000);

    cursorLine1Map[chart] = line1;
    cursorLine2Map[chart] = line2;

    qDebug() << "Curseurs ajoutés au graphique : " << chart;
}
void MainWindow::onCursorLineMoved(qreal x)
{
    qDebug() << "Curseur déplacé à x =" << x;

    if (!cursorLine1 || !cursorLine2)
        return;

    // Récupérer les positions x des deux curseurs
    qreal x1 = cursorLine1->pos().x();
    qreal x2 = cursorLine2->pos().x();

    qreal deltaX = qAbs(x2 - x1);

    // Si tu as un axe temps avec un scale, convertis deltaX en deltaT réel
    // Par exemple si ton axe X est en timestamp ms, alors deltaX est la différence temporelle en pixel
    // Tu dois convertir pixels -> unité temps, selon ton QValueAxis ou QDateTimeAxis

    qDebug() << "Delta X en pixels =" << deltaX;

    // TODO: conversion pixels -> delta temps (en ms ou s)
    // Par exemple :
    // qreal deltaT = pixelToTime(deltaX);

    // Afficher ou traiter deltaT
}
