#include <QtGlobal>
#include <cstdint>
#include "signal_model.h"
#include "can_decoder.h"

double decodeSignal(const QByteArray &data, const CanSignal &sig) {
    int startBit = sig.startBit;
    int length = sig.length;

    if (length <= 0 || startBit < 0 || startBit + length > data.size() * 8 || length > 64) {
        return 0.0;  // protection contre dépassement
    }

    uint64_t raw = 0;

    for (int i = 0; i < length; ++i) {
        int bitIndex = startBit + i;
        int byteIndex = bitIndex / 8;
        int bitPos = 7 - (bitIndex % 8); // Big endian bit order

        if (byteIndex >= data.size()) break;

        uint8_t bit = (data[byteIndex] >> bitPos) & 0x01;
        raw = (raw << 1) | bit;
    }

    if (sig.isSigned && length > 0) {
        uint64_t signBitMask = 1ULL << (length - 1);

        if (raw & signBitMask) {
            int64_t signedVal = static_cast<int64_t>(raw | (~0ULL << length));
            return sig.offset + signedVal * sig.factor;
        }
    }

    return sig.offset + raw * sig.factor;
}
