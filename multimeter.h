#pragma once

#include <QMainWindow>
#include <QBluetoothDeviceInfo>

#include "display.h"
#include "packetparser.h"

namespace Ui {
class Multimeter;
}

class Multimeter : public QMainWindow
{
    Q_OBJECT
public:
    explicit Multimeter(const QBluetoothDeviceInfo &device, QWidget *parent = nullptr);
    ~Multimeter();

    const QBluetoothDeviceInfo &getDeviceInfo() { return deviceInfo; }

signals:

private slots:
    void updateDisplay();

private:
    const QBluetoothDeviceInfo deviceInfo;
    Ui::Multimeter *ui;
    PacketParser parser;
};
