#ifndef DBC_JSON_PARSER_H
#define DBC_JSON_PARSER_H

#include <QString>
#include <QList>
#include "signal_model.h"

class DbcJsonParser {
public:
    DbcJsonParser() = default;
    bool loadFromFile(const QString &filePath);
    QList<Message> getMessages() const;
    const Message* getMessageById(int id) const;

private:
    QList<Message> messages;
};

#endif
