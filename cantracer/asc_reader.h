#ifndef ASC_READER_H
#define ASC_READER_H
#include "signal_model.h"
#include <QString>
#include <QList>
#include "can_frame.h"

class AscReader {
public:
    QList<CanFrame> readAscFile(const QString &filePath);
};

#endif
