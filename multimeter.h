#pragma once

#include <QMainWindow>
#include <QBluetoothDeviceInfo>
#include <QAbstractTableModel>
#include <QTime>

#include "display.h"
#include "packetparser.h"

namespace Ui {
class Multimeter;
}

class MultimeterValue {
public:
    explicit MultimeterValue(QTime _time, double _valueMain, double _valueSub) :
        time(_time), valueMain(_valueMain), valueSub(_valueSub) {}
    QTime time;
    double valueMain, valueSub;
};

class MultimeterValueModel : public QAbstractTableModel
{
public:
    MultimeterValueModel(QObject * parent = {}) :
        QAbstractTableModel{parent} {}

    void addValue(const MultimeterValue &value) {
        beginInsertRows({}, values.count(), values.count());
        values << value;
        endInsertRows();
    }

    void clear() {
        removeRows(0, values.count(), {});
        values.clear();
    }

    int rowCount(const QModelIndex &) const override { return values.count(); }
    int columnCount(const QModelIndex &) const override { return 4; }

    QVariant data(const QModelIndex &index, int role) const override {
          if (role != Qt::DisplayRole && role != Qt::EditRole)
              return {};

          const auto &value = values.at(index.row());

          switch (index.column()) {
          case 0:
              return index.row();
          case 1:
              return value.time.toString("hh:mm:ss.zzz");
          case 2:
              return QString::asprintf("%.10f", value.valueMain);
          case 3:
              return QString::asprintf("%.10f", value.valueSub);
          }

          return "foo";
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
          if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
              return {};

          switch (section) {
          case 0:
              return "#";
          case 1:
              return "Time";
          case 2:
              return "Value (main)";
          case 3:
              return "Value (sub)";
          default:
              return {};
          }
       }

private:
    QList<MultimeterValue> values;
};

class Multimeter : public QMainWindow
{
    Q_OBJECT
public:
    explicit Multimeter(const QBluetoothDeviceInfo &device, QWidget *parent = nullptr);
    ~Multimeter();

    const QBluetoothDeviceInfo &getDeviceInfo() { return deviceInfo; }

signals:

private slots:
    void handlePacket();

private:
    const QBluetoothDeviceInfo deviceInfo;
    Ui::Multimeter *ui;
    PacketParser parser;
    MultimeterValueModel valueModel;
};
