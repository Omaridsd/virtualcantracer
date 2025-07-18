#ifndef CAN_FRAME_H
#define CAN_FRAME_H

#include <QByteArray>

struct CanFrame {
    int id;
    QByteArray data;
    double timestamp; // Optionnel
};

#endif // CAN_FRAME_H
