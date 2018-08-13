#pragma once

#include <QObject>

class PacketParser : public QObject
{
    Q_OBJECT
public:
    explicit PacketParser(QObject *parent = nullptr);
    bool parse(const QByteArray &data);

    enum Icon {
        Battery     = 1 << 0,
        Apo         = 1 << 1,
        Auto        = 1 << 2,
        MainAC      = 1 << 3,
        MainDC      = 1 << 4,
        OneMs       = 1 << 5,
        LowPass     = 1 << 6,
        Celcius     = 1 << 7,

        Avg         = 1 << 8,
        Max         = 1 << 9,
        Min         = 1 << 10,
        Dbm         = 1 << 11,
        Rel         = 1 << 12,
        Down        = 1 << 13,
        BT          = 1 << 14,
        Fahrenheit  = 1 << 15,

        DC          = 1 << 16,
        AC          = 1 << 17,
        Hold        = 1 << 18,
        AMinus      = 1 << 19,
        Mem         = 1 << 20,
        Test        = 1 << 22,
        Zero        = 1 << 23,
    };
    Q_DECLARE_FLAGS(Icons, Icon);

    Icons getIcons() { return currentIcons; }
    uint32_t getSerialNumber() { return serialNumber; }
    uint8_t getBarValue() { return barValue; }

private:
    uint32_t serialNumber;
    uint8_t barValue;
    Icons currentIcons;
};
