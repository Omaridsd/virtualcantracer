#include "asc_reader.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include "can_frame.h"
QList<CanFrame> AscReader::readAscFile(const QString &filePath) {
    QList<CanFrame> frames;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Impossible d'ouvrir le fichier ASC :" << filePath;
        return frames;
    }

    QTextStream in(&file);
    QRegularExpression regex(R"(^\s*(\d+)\s+0x([0-9A-Fa-f]+)\s+(\d+)\s+((?:[0-9A-Fa-f]{2}\s+){1,8})(RX|TX))");

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

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
                    qWarning() << "Octet invalide dans la ligne:" << line;
                    break;
                }
            }

            frames.append(frame);
        } else {
            qWarning() << "Ligne non reconnue et ignorée:" << line;
        }
    }

    qDebug() << "Nombre total de trames lues:" << frames.size();
    return frames;
}
