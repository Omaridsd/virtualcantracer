#include "asc_reader.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include "can_frame.h"

QList<CanFrame> AscReader::readAscFile(const QString &filePath) {
    QList<CanFrame> frames;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "❌ Impossible d'ouvrir le fichier ASC :" << filePath;
        return frames;
    }

    QTextStream in(&file);
    bool inTriggerBlock = false;

    // Expression régulière pour les trames CAN du format Vector ASC
    QRegularExpression regex(R"(^\s*(\d+\.\d+)\s+\d+\s+([0-9A-Fa-f]+)\s+Rx\s+d\s+(\d)\s+((?:[0-9A-Fa-f]{2}\s+){0,8}))");

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        // Ignorer l'en-tête du fichier jusqu'à "BeginTriggerblock"
        if (!inTriggerBlock) {
            if (line.contains("BeginTriggerblock")) {
                inTriggerBlock = true;
            }
            continue;
        }

        QRegularExpressionMatch match = regex.match(line);
        if (match.hasMatch()) {
            CanFrame frame;
            frame.timestamp = match.captured(1).toDouble();
            frame.id = match.captured(2).toInt(nullptr, 16);

            QString dataStr = match.captured(4).trimmed();
            QStringList byteStrings = dataStr.split(' ', Qt::SkipEmptyParts);

            for (const QString &byteStr : byteStrings) {
                bool ok;
                uint8_t byte = static_cast<uint8_t>(byteStr.toUInt(&ok, 16));
                if (ok) {
                    frame.data.append(static_cast<char>(byte));
                } else {
                    qWarning() << "❌ Octet invalide dans la ligne:" << line;
                    break;
                }
            }

            frames.append(frame);
        } else {
            // Ligne ignorée si elle ne correspond pas à une trame
            // qWarning() << "Ligne non reconnue:" << line;
        }
    }

    qDebug() << "✅ Nombre total de trames lues:" << frames.size();
    return frames;
}
QDateTime AscReader::readAscStartDate(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QDateTime();

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (line.startsWith("date ")) {
            QString dateStr = line.mid(5).trimmed();

            QStringList formats = {
                "MM/dd/yy HH:mm:ss.zzz",
                "yyyy-MM-dd HH:mm:ss.zzz",
                "MM/dd/yy HH:mm:ss",
                "yyyy-MM-dd HH:mm:ss"
            };

            for (const QString& format : formats) {
                QDateTime dt = QDateTime::fromString(dateStr, format);
                if (dt.isValid()) {
                    dt.setTimeSpec(Qt::LocalTime); // ou Qt::UTC selon besoin
                    return dt;
                }
            }

            qWarning() << "❌ Format de date non reconnu:" << dateStr;
            break; // inutile de continuer si on a trouvé "date"
        }

        if (in.pos() > 1000) break; // pour éviter de lire tout le fichier
    }

    qWarning() << "❌ Aucune date trouvée dans l'en-tête du fichier ASC.";
    return QDateTime();
}
QDateTime AscReader::getStartDateTime() const {
    return startDateTime;
}
