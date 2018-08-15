#pragma once

#include <QObject>

class PacketParser : public QObject
{
    Q_OBJECT
public:
    enum PacketVersion {
        PacketVersion2,
    };

    explicit PacketParser(PacketVersion version, QObject *parent = nullptr);
    bool parse(const QByteArray &data);

    enum MainRangeFlag {
        Fahrenheit  = 1 << 0,
        Celcius     = 1 << 1,
        Negative    = 1 << 2,
        OFL         = 1 << 3,
    };
    Q_DECLARE_FLAGS(MainRangeFlags, MainRangeFlag);

    enum Icon {
        Battery     = 1 << 0,
        Apo         = 1 << 1,
        Auto        = 1 << 2,
        MainAC      = 1 << 3,
        MainDC      = 1 << 4,
        OneMs       = 1 << 5,
        LowPass     = 1 << 6,
        IconCelcius = 1 << 7,

        Avg         = 1 << 8,
        Max         = 1 << 9,
        Min         = 1 << 10,
        dBm         = 1 << 11,
        Rel         = 1 << 12,
        Down        = 1 << 13,
        BT          = 1 << 14,
        IconFahrenheit  = 1 << 15,

        DC          = 1 << 16,
        AC          = 1 << 17,
        Hold        = 1 << 18,
        AMinus      = 1 << 19,
        Mem         = 1 << 20,
        Test        = 1 << 22,
    };
    Q_DECLARE_FLAGS(Icons, Icon);

    enum BarFlag {
        Scale500    = 1 << 0,
        Scale1000   = 1 << 1,
        BarNegative = 1 << 2,
        Dunno       = 1 << 3,
        Use         = 1 << 4,
    };
    Q_DECLARE_FLAGS(BarFlags, BarFlag);

    MainRangeFlags getMainRangeFlags() { return mainRangeFlags; }

    Icons getIcons() { return currentIcons; }
    uint32_t getSerialNumber() { return serialNumber; }

    BarFlags getBarFlags() { return barFlags; }
    uint8_t getBarValue() { return barValue; }

private:
    PacketVersion packetVersion;
    uint32_t serialNumber;

    BarFlags barFlags;
    uint8_t barValue;

    MainRangeFlags mainRangeFlags;
    Icons currentIcons;

    bool parseV2(const QByteArray &data);
};
