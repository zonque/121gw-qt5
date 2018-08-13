#include "packetparser.h"

struct PacketV2
{
    uint32_t serial;
    uint8_t mainMode;
    uint8_t mainRange;
    uint16_t mainValue;
    uint8_t subMode;
    uint8_t subRange;
    uint16_t subValue;
    uint8_t barStatus;
    uint8_t barValue;
    uint8_t iconStatus[3];
    uint8_t checksum;
} __attribute__((packed));

PacketParser::PacketParser(QObject *parent) : QObject(parent)
{
}

bool PacketParser::parse(const QByteArray &data)
{
    const struct PacketV2 *packet;

    if (sizeof(*packet) != data.length())
        return false;

    // FIXME: check checksum!

    packet = (const struct PacketV2 *) data.constData();

    barValue = packet->barValue;

    currentIcons = (Icons)
            ((packet->iconStatus[0]) |
             (packet->iconStatus[1] << 8) |
             (packet->iconStatus[2] << 16));

    return true;
}
