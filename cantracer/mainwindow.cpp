#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include "dbc_json_parser.h"
#include "asc_reader.h"
#include "signal_model.h"
#include "can_decoder.h"

// Déclaration de la fonction de décodage
double decodeSignal(const QByteArray &data, const CanSignal &sig);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tableModel = new QStandardItemModel(this);

    connect(ui->btnloadjson, &QPushButton::clicked, this, &MainWindow::onLoadJson);
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
            qDebug() << "✅ Fichier JSON chargé avec succès : " << filePath;
        } else {
            qDebug() << "❌ Erreur lors du chargement du fichier JSON.";
        }
    } else {
        qDebug() << "ℹ️ Aucun fichier sélectionné.";
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
