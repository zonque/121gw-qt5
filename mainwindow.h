#pragma once

#include <QMainWindow>
#include <QBluetoothDeviceInfo>
#include <QBluetoothServiceDiscoveryAgent>

#include "multimeter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void scan();
    void scanEnded();

private:
    QList<Multimeter*> multimeters;
    QList<QBluetoothDeviceInfo> detectedDevices;
    Ui::MainWindow *ui;
    QBluetoothDeviceDiscoveryAgent *deviceDiscoveryAgent;
};
