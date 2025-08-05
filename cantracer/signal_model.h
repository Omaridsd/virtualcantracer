#ifndef SIGNAL_MODEL_H
#define SIGNAL_MODEL_H

#include <QString>
#include <QList>

// Structure représentant un signal dans un message CAN
struct CanSignal {
    QString name;      // Nom du signal
    int startBit;      // Position du bit de départ
    int length;        // Longueur du signal en bits
    QString unit;      // Unité du signal (ex: km/h, rpm)
    double factor;     // Facteur de conversion
    double offset;
    bool isSigned=false ;    // Décalage
    bool isBigEndian = false;
    int messageId = -1;  // ✅ ID du message parent

};

// Structure représentant un message CAN avec ses signaux
struct Message {
    QString name;           // Nom du message
    int id;                 // Identifiant du message CAN
    QList<CanSignal> signalss;  // Liste des signaux contenus dans le message
};

#endif // SIGNAL_MODEL_H
