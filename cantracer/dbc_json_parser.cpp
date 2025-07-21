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

    // Ton JSON est un tableau au top-level, pas un objet
    QJsonArray messagesArray = doc.array();

    messages.clear();

    for (const QJsonValue &msgVal : messagesArray) {
        QJsonObject msgObj = msgVal.toObject();

        Message msg;
        msg.name = msgObj["name"].toString();
        msg.id = msgObj["frame_id"].toInt();  // Utilise frame_id ici

        QJsonArray signalsArray = msgObj["signals"].toArray();
        for (const QJsonValue &sigVal : signalsArray) {
            QJsonObject sigObj = sigVal.toObject();

            CanSignal sig;
            sig.name = sigObj["name"].toString();
            sig.startBit = sigObj["start"].toInt();  // champ 'start' dans JSON
            sig.length = sigObj["length"].toInt();
            sig.factor = sigObj["factor"].toDouble();
            sig.offset = sigObj["offset"].toDouble();

            // unit peut être null, gérer ça
            if (sigObj["unit"].isNull())
                sig.unit = "";
            else
                sig.unit = sigObj["unit"].toString();

            // Facultatif : récupérer ces champs si besoin
            sig.isBigEndian = sigObj["is_big_endian"].toBool();
            sig.isSigned = sigObj["is_signed"].toBool();

            msg.signalss.append(sig);  // corriger le nom de la liste dans Message
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


