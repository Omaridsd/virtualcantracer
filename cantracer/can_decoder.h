
#ifndef CAN_DECODER_H
#define CAN_DECODER_H

#include <QByteArray>
#include "signal_model.h"

double decodeSignal(const QByteArray &data, const CanSignal &sig);

#endif // CAN_DECODER_H
