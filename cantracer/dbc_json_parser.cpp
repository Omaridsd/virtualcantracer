#include "dbc_json_parser.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include "signal_model.h"

bool DbcJsonParser::loadFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Impossible d'ouvrir le fichier JSON.";
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Erreur de parsing JSON :" << error.errorString();
        return false;
    }

    QJsonObject root = doc.object();
    QJsonArray messagesArray = root["messages"].toArray();

    messages.clear();  // Nettoyer l'ancien contenu

    for (const QJsonValue &msgVal : messagesArray) {
        QJsonObject msgObj = msgVal.toObject();

        Message msg;
        msg.name = msgObj["name"].toString();
        msg.id = msgObj["id"].toInt();

        QJsonArray signalsArray = msgObj["signals"].toArray();
        for (const QJsonValue &sigVal : signalsArray) {
            QJsonObject sigObj = sigVal.toObject();

            CanSignal sig;
            sig.name = sigObj["name"].toString();
            sig.startBit = sigObj["startBit"].toInt();
            sig.length = sigObj["length"].toInt();
            sig.unit = sigObj["unit"].toString();
            sig.factor = sigObj["factor"].toDouble();
            sig.offset = sigObj["offset"].toDouble();
            msg.signalss.append(sig);
        }

        messages.append(msg);
    }

    qDebug() << "✅ Nombre de messages chargés :" << messages.size();
    return true;
}

QList<Message> DbcJsonParser::getMessages() const {
    return messages;
}
const Message* DbcJsonParser::getMessageById(int id) const {
    for (const Message &msg : messages) {
        if (msg.id == id)
            return &msg;
    }
    return nullptr;
}


