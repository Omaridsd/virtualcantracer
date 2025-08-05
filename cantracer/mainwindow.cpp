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
//#include "signal_selection_dialog.h"
#include <QToolTip>
#include <QtCharts/QDateTimeAxis>
#include <QColorDialog>
#include <QtCharts/QScatterSeries>
#include "movablecursorline.h"
#include <QTimer>
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
    CustomChartView* chartView = new CustomChartView(this);
    chartView->setChart(chart);
    chartView->setRubberBand(QChartView::RectangleRubberBand);
    chartView->setRenderHint(QPainter::Antialiasing);
    connect(chartView, &CustomChartView::cursorMoved, this, [this, chartView](qreal xPos, int cursorId){
        this->onCursorMoved(xPos, cursorId, chartView);
    });
    // Créer les curseurs
    qreal xStart = chart->plotArea().left() + 50;
    qreal xEnd = chart->plotArea().left() + 150;

    QLineF line1(xStart, chart->plotArea().top(), xStart, chart->plotArea().bottom());
    QLineF line2(xEnd, chart->plotArea().top(), xEnd, chart->plotArea().bottom());

    QGraphicsLineItem* cursor1 = new QGraphicsLineItem(line1);
    QGraphicsLineItem* cursor2 = new QGraphicsLineItem(line2);
    cursor1->setPen(QPen(Qt::black, 1, Qt::DashLine));
    cursor2->setPen(QPen(Qt::black, 1, Qt::DashLine));

    chartView->scene()->addItem(cursor1);
    chartView->scene()->addItem(cursor2);

    chartView->setCursors(cursor1, cursor2);
    chartView->enableCursors(true);

    // Connexion unique au moment de la création du graphique
    QValueAxis* axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
    if (axisX) {
        connect(axisX, &QValueAxis::rangeChanged, this, [this, axisX](qreal min, qreal max) {
            // Synchroniser la plage X sur tous les autres graphiques
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

    //chart->axes(Qt::Horizontal).first()->setTitleText("Temps (s)");
    //chart->axes(Qt::Vertical).first()->setTitleText("Valeur");
    chart->setTitle(signalName);  // sert à identifier le graphique pour pouvoir le retirer ensuite
    QTimer::singleShot(0, this, [=]() {
        addCursorLinesToChart(chartView);
    });
    graphsLayout->addWidget(chartView);
    signalChartViewsMap[signalName] = chartView;  // <-- Ici

    chartView->setCursor(Qt::PointingHandCursor);
    chartView->installEventFilter(this);
    chart->setMargins(QMargins(0, 0, 0, 0));             // Supprime les marges internes
    chart->legend()->hide();                             // Cache la légende
    chart->setBackgroundRoundness(0);                    // Coins non arrondis si utile
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
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        // ❗️Seulement clic gauche
        if (mouseEvent->button() == Qt::LeftButton) {
            QChartView* clickedChart = qobject_cast<QChartView*>(obj);
            if (clickedChart) {
                // Vérifie s’il y a au moins un signal coché dans le QTreeWidget
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

                if (foundChecked) {
                    selectedChartView = clickedChart;
                    qDebug() << "✅ Graphique sélectionné (clic gauche sur n'importe quelle zone).";
                } else {
                    qDebug() << "⚠️ Aucun signal coché.";
                }
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
void MainWindow::addCursorLinesToChart(QChartView* chartView) {
    QChart* chart = chartView->chart();

    // Créer deux lignes verticales initiales aux positions X différentes
    qreal x1 = chart->plotArea().left() + 50;
    qreal x2 = chart->plotArea().left() + 150;

    QLineF line1(x1, chart->plotArea().top(), x1, chart->plotArea().bottom());
    QLineF line2(x2, chart->plotArea().top(), x2, chart->plotArea().bottom());

    /*auto* lineItem1 = new QGraphicsLineItem(line1);
    auto* lineItem2 = new QGraphicsLineItem(line2);

    lineItem1->setPen(QPen(Qt::black, 1, Qt::DashLine));
    lineItem2->setPen(QPen(Qt::black, 1, Qt::DashLine));

    chartView->scene()->addItem(lineItem1);
    chartView->scene()->addItem(lineItem2);*/

    // Enregistre les curseurs pour ce graphe (si tu veux les manipuler plus tard)
    //cursorMap[chartView] = std::make_pair(lineItem1, lineItem2);
}
void MainWindow::onCursorMoved(qreal xPos, int cursorId, CustomChartView* sourceChartView) {
    // Met à jour tous les autres graphiques sauf celui d'origine
    for (int i = 0; i < graphsLayout->count(); ++i) {
        QWidget* widget = graphsLayout->itemAt(i)->widget();
        CustomChartView* chartView = qobject_cast<CustomChartView*>(widget);
        if (chartView && chartView != sourceChartView) {
            QGraphicsLineItem* cursorToMove = (cursorId == 1) ? chartView->getCursor1() : chartView->getCursor2();
            QLineF newLine(xPos,
                           chartView->chart()->plotArea().top(),
                           xPos,
                           chartView->chart()->plotArea().bottom());
            cursorToMove->setLine(newLine);
        }
    }

    // Récupérer la position X des deux curseurs en coordonnées "valeurs" (ex: secondes)
    qreal x1 = sourceChartView->chart()->mapToValue(sourceChartView->getCursor1()->line().p1()).x();
    qreal x2 = sourceChartView->chart()->mapToValue(sourceChartView->getCursor2()->line().p1()).x();

    qreal deltaT = std::abs(x2 - x1);

    qDebug() << "Delta T (secondes) :" << deltaT;

    // Si tu as un QLabel* labelDeltaT dans ton UI, mets à jour son texte
    if (ui->labelDeltaT) {
        ui->labelDeltaT->setText(QString("Delta T : %1 s").arg(deltaT, 0, 'f', 3));
    }
}
