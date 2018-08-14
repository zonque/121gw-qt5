#include "multimeter.h"
#include <QLowEnergyController>
#include <QBluetoothUuid>
#include <QFileDialog>

#include "ui_multimeter.h"

Multimeter::Multimeter(const QBluetoothDeviceInfo &device, QWidget *parent) :
    QMainWindow(parent),
    deviceInfo(device),
    ui(new Ui::Multimeter),
    parser(PacketParser::PacketVersion2, this),
    valueModel()
{
    ui->setupUi(this);

    QLowEnergyController *c = new QLowEnergyController(device, this);

    QObject::connect(c,&QLowEnergyController::connected, [this, c]() {
        c->discoverServices();
    });

    QObject::connect(c,&QLowEnergyController::serviceDiscovered, [this, c](const QBluetoothUuid &uuid) {
        if (uuid != QBluetoothUuid(QString("0bd51666-e7cb-469b-8e4d-2742f1ba77cc")))
            return;

        QLowEnergyService *s = c->createServiceObject(uuid);
        QObject::connect(s, &QLowEnergyService::characteristicChanged, [this](const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue) {
            Q_UNUSED(characteristic);

            if (parser.parse(newValue))
                handlePacket();
        });

        QObject::connect(s, &QLowEnergyService::stateChanged, [this, s](QLowEnergyService::ServiceState newState) {
            if (newState == QLowEnergyService::ServiceDiscovered) {
                foreach (const QLowEnergyCharacteristic characteristic, s->characteristics()) {
                    // Look for the ClientCharacteristicConfiguration descriptor in all characteristics, and
                    // send the magic 0x0100 value to each of them. This enables the asynchronous
                    // characteristic updates.

                    QLowEnergyDescriptor m_notificationDesc = characteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
                    if (m_notificationDesc.isValid())
                        s->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));
                }
            }
        });

        s->discoverDetails();
    });

    c->connectToDevice();

    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setModel(&valueModel);

    QObject::connect(ui->historyClearButton, &QPushButton::clicked, [this]() {
        ui->tableView->scrollToTop();
        valueModel.clear();
    });

    QObject::connect(ui->historySaveButton, &QPushButton::clicked, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this,
            "Save values as CSV", NULL, "CSV (*.csv *.txt)");
        // ...
    });

    setWindowTitle(device.name() + " (" + device.address().toString() + ")");
}

Multimeter::~Multimeter()
{
    delete ui;
}

void Multimeter::handlePacket()
{
    Display::Icons icons = 0;

    // FIXME
    valueModel.addValue(MultimeterValue(QTime::currentTime(), 0.1, 0.2));
    ui->tableView->scrollToBottom();

    if (parser.getIcons() & PacketParser::Icon::Battery)
        icons |= Display::Icon::Battery;

    if (parser.getIcons() & PacketParser::Icon::Apo)
        icons |= Display::Icon::Apo;

    if (parser.getIcons() & PacketParser::Icon::Auto)
        icons |= Display::Icon::Auto;

    //...
    if (parser.getIcons() & PacketParser::Icon::OneMs)
        icons |= Display::Icon::OneMs;

    if (parser.getIcons() & PacketParser::Icon::LowPass)
        icons |= Display::Icon::LowPass;

    //...

    if (parser.getIcons() & PacketParser::Icon::Avg)
        icons |= Display::Icon::Avg;

    if (parser.getIcons() & PacketParser::Icon::Max)
        icons |= Display::Icon::Max;

    if (parser.getIcons() & PacketParser::Icon::Min)
        icons |= Display::Icon::Min;

    //...

    if (parser.getIcons() & PacketParser::Icon::BT)
        icons |= Display::Icon::BT;

    //...

    if (parser.getBarFlags() & PacketParser::BarFlag::Use) {
        if (parser.getBarFlags() & PacketParser::BarFlag::Scale500)
            ui->display->setBarStatus(Display::BarStatus::On500);

        if (parser.getBarFlags() & PacketParser::BarFlag::Scale1000)
            ui->display->setBarStatus(Display::BarStatus::On1000);

        ui->display->setBarValue(parser.getBarValue());
        ui->display->setBarNegative(parser.getBarFlags() & PacketParser::BarFlag::Negative);
    } else
        ui->display->setBarStatus(Display::BarStatus::Off);


    qInfo() << __func__ << icons;
    ui->display->setIcons(icons);
}
