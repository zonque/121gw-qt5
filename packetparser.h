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

    enum Mode {
        ModeLowZ               = 0,
        ModeDCV                = 1,
        ModeACV                = 2,
        ModeDCmV               = 3,
        ModeACmV               = 4,
        ModeTemp               = 5,
        ModeHz                 = 6,
        ModemS                 = 7,
        ModeDuty               = 8,
        ModeResistor           = 9,
        ModeContinuity         = 10,
        ModeDiode              = 11,
        ModeCapacitor          = 12,
        ModeACuVA              = 13,
        ModeACmVA              = 14,
        ModeACVA               = 15,
        ModeACuA               = 16,
        ModeDCuA               = 17,
        ModeACmA               = 18,
        ModeDCmA               = 19,
        ModeACA                = 20,
        ModeDCA                = 21,
        ModeDCuVA              = 22,
        ModeDCmVA              = 23,
        ModeDCVA               = 24,
        ModeSetupTempC         = 100,
        ModeSetupTempF         = 105,
        ModeSetupBattery       = 110,
        ModeSetupAPO_On        = 120,
        ModeSetupAPO_Off       = 125,
        ModeSetupYear          = 130,
        ModeSetupDate          = 135,
        ModeSetupTime          = 140,
        ModeSetupBurdenVoltage = 150,
        ModeSetupLCD           = 160,
        ModeSetupdBm           = 180,
        ModeSetupInterval      = 190
    };

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
        MainDC      = 1 << 3,
        MainAC      = 1 << 4,
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

    enum BarScale {
        BarScale5    = 0,
        BarScale50   = 1,
        BarScale500  = 2,
        BarScale1000 = 3,
    };

    enum BarFlag {
        BarNegative = 1 << 2,
        Lower       = 1 << 3,
        Use         = 1 << 4,
    };
    Q_DECLARE_FLAGS(BarFlags, BarFlag);

    Mode getMainMode() { return mainMode; }
    uint32_t getMainValue() { return mainValue; }
    uint32_t getMainScale() { return mainScale; }
    MainRangeFlags getMainRangeFlags() { return mainRangeFlags; }

    Mode getSubMode() { return subMode; }
    uint16_t getSubValue() { return subValue; }

    Icons getIcons() { return currentIcons; }
    uint32_t getSerialNumber() { return serialNumber; }

    BarFlags getBarFlags() { return barFlags; }
    BarScale getBarScale() { return barScale; }
    uint8_t getBarValue() { return barValue; }

private:
    PacketVersion packetVersion;
    uint32_t serialNumber;

    Mode mainMode, subMode;

    BarFlags barFlags;
    BarScale barScale;
    uint8_t barValue;

    uint32_t mainValue;
    uint32_t mainScale;
    MainRangeFlags mainRangeFlags;

    uint16_t subValue;

    Icons currentIcons;

    bool parseV2(const QByteArray &data);
};
