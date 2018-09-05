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
    Display::UnitIcons unitIcons = 0;

    // FIXME: scaling is not implemented

    double mainValue, subValue;

    switch (parser.getMainMode()) {
    case PacketParser::Mode::ModeLowZ:
        icons |= Display::Icon::LowZ;
        unitIcons |= Display::UnitIcon::mainV;
        break;
    case PacketParser::Mode::ModeDCmV:
    case PacketParser::Mode::ModeACmV:
        unitIcons |= Display::UnitIcon::mainMilli;
        // fall-through
    case PacketParser::Mode::ModeDCV:
    case PacketParser::Mode::ModeACV:
        unitIcons |= Display::UnitIcon::mainV;
    default:
        break;
    }

    switch (parser.getSubMode()) {
    case PacketParser::Mode::ModeDCmV:
    case PacketParser::Mode::ModeACmV:
        unitIcons |= Display::UnitIcon::subMilli;
        // fall-through
    case PacketParser::Mode::ModeDCV:
    case PacketParser::Mode::ModeACV:
        unitIcons |= Display::UnitIcon::subV;
    default:
        break;
    }

    if (parser.getMainRangeFlags() & PacketParser::MainRangeFlag::OFL) {
        ui->display->setMain("OFL ", 3);
        mainValue = qInf();
    } else {
        ui->display->setMain(QString::asprintf("%05d", parser.getMainValue()), 0);
        mainValue = parser.getMainValue();
    }

    ui->display->setSub(QString::asprintf("%05d", parser.getSubValue()), 0);
    subValue = parser.getSubValue();

    valueModel.addValue(MultimeterValue(QTime::currentTime(), mainValue, subValue));
    ui->tableView->scrollToBottom();

    if (parser.getMainRangeFlags() & PacketParser::MainRangeFlag::Fahrenheit)
        unitIcons |= Display::UnitIcon::mainFahrenheit;

    if (parser.getMainRangeFlags() & PacketParser::MainRangeFlag::Celcius)
        unitIcons |= Display::UnitIcon::mainCelcius;

    if (parser.getIcons() & PacketParser::Icon::Battery)
        icons |= Display::Icon::Battery;

    if (parser.getIcons() & PacketParser::Icon::Apo)
        icons |= Display::Icon::Apo;

    if (parser.getIcons() & PacketParser::Icon::Auto)
        icons |= Display::Icon::Auto;

    if (parser.getIcons() & PacketParser::Icon::MainDC)
        icons |= Display::Icon::MainDC;

    if (parser.getIcons() & PacketParser::Icon::MainAC) {
        icons |= Display::Icon::MainAC;
        if (parser.getIcons() & PacketParser::Icon::MainDC)
            icons |= Display::Icon::MainDCPlusAC;
    }

    //...
    if (parser.getIcons() & PacketParser::Icon::OneMs)
        icons |= Display::Icon::OneMs;

    if (parser.getIcons() & PacketParser::Icon::LowPass)
        icons |= Display::Icon::LowPass;

    //...
    if (parser.getIcons() & PacketParser::Icon::dBm)
        unitIcons |= Display::UnitIcon::subdB;

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
    if (parser.getIcons() & PacketParser::Icon::DC)
        icons |= Display::Icon::DC;

    if (parser.getIcons() & PacketParser::Icon::AC)
        icons |= Display::Icon::AC;

    if (parser.getIcons() & PacketParser::Icon::Hold)
        icons |= Display::Icon::Hold;

    if (parser.getIcons() & PacketParser::Icon::AMinus)
        icons |= Display::Icon::AMinus;

    if (parser.getIcons() & PacketParser::Icon::Mem)
        icons |= Display::Icon::Mem;

    if (parser.getIcons() & PacketParser::Icon::Test)
        icons |= Display::Icon::Test;

    if (!(parser.getBarFlags() & PacketParser::BarFlag::Use)) {
        switch (parser.getBarScale()) {
        case PacketParser::BarScale5:
            ui->display->setBarStatus(Display::BarStatus::On5);
            break;
        case PacketParser::BarScale50:
            ui->display->setBarStatus(Display::BarStatus::On50);
            break;
        case PacketParser::BarScale500:
            ui->display->setBarStatus(Display::BarStatus::On500);
            break;
        case PacketParser::BarScale1000:
            ui->display->setBarStatus(Display::BarStatus::On1000);
            break;
        }

        ui->display->setBarValue(parser.getBarValue(), parser.getBarFlags() & PacketParser::BarFlag::BarNegative);
    } else
        ui->display->setBarStatus(Display::BarStatus::Off);

    qInfo() << __func__ << icons;

    ui->display->setIcons(icons);
    ui->display->setUnitIcons(unitIcons);
}
