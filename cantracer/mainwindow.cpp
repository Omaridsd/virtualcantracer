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

// Déclaration de la fonction de décodage
double decodeSignal(const QByteArray &data, const CanSignal &sig);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tableModel = new QStandardItemModel(this);
    connect(ui->tableviewsignals, &QTableView::clicked, this, &MainWindow::onSignalSelected);
    connect(ui->btnChooseSignals, &QPushButton::clicked, this, [this]() {
        // Liste des signaux disponibles (extrait depuis le DBC par exemple)
        QStringList allSignals;
        for (const auto &msg : parser.getMessages()) {
            for (const auto &sig : msg.signalss) {
                allSignals << sig.name;
            }
        }

        SignalSelectionDialog dialog(allSignals, this);
        if (dialog.exec() == QDialog::Accepted) {
            QStringList selected = dialog.selectedSignals();
            qDebug() << "✔️ Signaux sélectionnés pour la courbe :" << selected;
            // TODO : mettre à jour les courbes avec ces signaux
            QChart *chart = new QChart();
            chart->setTitle("Évolution des signaux sélectionnés");
            chart->legend()->setVisible(true);
            chart->legend()->setAlignment(Qt::AlignBottom);

            // Pour chaque signal sélectionné
            for (const QString &sigName : selected) {
                QLineSeries *series = new QLineSeries();
                series->setName(sigName);  // Nom visible dans la légende

                // Parcourir les lignes du tableau
                for (int row = 0; row < tableModel->rowCount(); ++row) {
                    QString rowSignal = tableModel->item(row, 3)->text();
                    if (rowSignal == sigName) {
                        double timestamp = tableModel->item(row, 0)->text().toDouble();
                        double value = tableModel->item(row, 4)->text().toDouble();
                        series->append(timestamp, value);
                    }
                }

                chart->addSeries(series);
            }

            chart->createDefaultAxes();
            chart->axes(Qt::Horizontal).first()->setTitleText("Temps (s)");
            chart->axes(Qt::Vertical).first()->setTitleText("Valeur");

            // Afficher dans le QChartView promu
            ui->chartContainer->setChart(chart);

        }
    });
    connect(ui->btnloaddbc, &QPushButton::clicked, this, &MainWindow::onLoadDbc);
    connect(ui->btnloadasc, &QPushButton::clicked, this, &MainWindow::onLoadAsc);
    tableModel->setHorizontalHeaderLabels(QStringList()
                                          << "Timestamp" << "ID" << "Message" << "Signal" << "Value" << "Unit");
    ui->tableviewsignals->setModel(tableModel);

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
            ui->labelStatus->setText("✅ Fichier JSON chargé avec succès : " + filePath);
        } else {
            ui->labelStatus->setText("❌ Erreur lors du chargement du fichier JSON.");
        }
    } else {
        ui->labelStatus->setText("ℹ️ Aucun fichier sélectionné.");
    }
}

void MainWindow::onLoadAsc()
{
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
                QList<QStandardItem*> row;
                row << new QStandardItem(QString::number(frame.timestamp))                     // Timestamp
                    << new QStandardItem(QString("0x%1").arg(frame.id, 0, 16).toUpper())       // ID
                    << new QStandardItem(msg->name)                                            // Message name
                    << new QStandardItem(sig.name)                                             // Signal name
                    << new QStandardItem(QString::number(value))                               // Signal value
                    << new QStandardItem(sig.unit);                                            // Unit

                tableModel->appendRow(row);
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
    dir.cdUp(); // from Debug folder to build
    dir.cdUp(); // from build to cantracer
    dir.cd("python"); // into python folder

    QString scriptPath = dir.filePath("dbc_to_json.py");

    QString pythonExe = "C:/Users/DELL/AppData/Local/Programs/Thonny/python.exe";
    qDebug() << "✅ Python utilisé par Qt:" << pythonExe;

    if (pythonExe.isEmpty()) {
        qWarning() << "❌ Python non trouvé dans PATH.";
        return false;
    }

    process.start(pythonExe, QStringList() << scriptPath << dbcPath << jsonOutPath);
    process.waitForFinished();

    QByteArray stdOut = process.readAllStandardOutput();
    QByteArray stdErr = process.readAllStandardError();

    if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0) {
        qDebug() << "✅ Conversion réussie :" << jsonOutPath;
        qDebug() << "stdout:" << stdOut;
        return true;
    } else {
        qWarning() << "❌ Erreur conversion DBC -> JSON :";
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

    // Balayer toutes les lignes du tableau pour ce signal
    for (int row = 0; row < tableModel->rowCount(); ++row) {
        QString rowSignal = tableModel->item(row, 3)->text();
        QString rowMessage = tableModel->item(row, 2)->text();
        if (rowSignal == signalName && rowMessage == messageName) {
            double timestamp = tableModel->item(row, 0)->text().toDouble();
            double value = tableModel->item(row, 4)->text().toDouble();
            series->append(timestamp, value);
        }
    }

    // Création du graphique
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Évolution du signal : " + signalName);
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).first()->setTitleText("Temps (s)");
    chart->axes(Qt::Vertical).first()->setTitleText("Valeur");

    // Affichage dans le QChartView promu
    ui->chartContainer->setChart(chart);
}
