#ifndef ASC_READER_H
#define ASC_READER_H
#include "signal_model.h"
#include <QString>
#include <QList>
#include "can_frame.h"
#include <QDateTime>

class AscReader {
public:
    AscReader() = default;
    QDateTime getStartDateTime() const;

    QList<CanFrame> readAscFile(const QString &filePath);
   static  QDateTime readAscStartDate(const QString& filePath);
private:
    QList<CanFrame> frames;
    QDateTime startDateTime;
};

#endif
