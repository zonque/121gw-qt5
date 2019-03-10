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

    QObject::connect(c,&QLowEnergyController::connected, [c]() {
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

        QObject::connect(s, &QLowEnergyService::stateChanged, [s](QLowEnergyService::ServiceState newState) {
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
            "Save values as CSV", nullptr, "CSV (*.csv *.txt)");
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
    Display::Icons icons;
    Display::UnitIcons unitIcons;
    int mainDpPosition = 0;
    double mainScaleFactor = 1.0;
    mainScaleFactor = 10.0;

    double mainValue, subValue;

    switch (parser.getMainMode()) {
    case PacketParser::Mode::ModeLowZ:
        icons |= Display::Icon::LowZ;
        unitIcons |= Display::UnitIcon::mainV;
        mainDpPosition = 1;
        mainScaleFactor = 10.0;
        break;

    case PacketParser::Mode::ModeDCmV:
    case PacketParser::Mode::ModeACmV:
        unitIcons |= Display::UnitIcon::mainMilli;
        unitIcons |= Display::UnitIcon::mainV;

        switch (parser.getMainScale()) {
        case 0:
            mainDpPosition = 3;
            mainScaleFactor = 0.000001;
            break;
        case 1:
            mainDpPosition = 2;
            mainScaleFactor = 0.00001;
            break;
        }

        break;

    case PacketParser::Mode::ModeDCV:
    case PacketParser::Mode::ModeACV:
        unitIcons |= Display::UnitIcon::mainV;

        switch (parser.getMainScale()) {
        case 0:
            mainDpPosition = 4;
            mainScaleFactor = 0.0001;
            break;
        case 1:
            mainDpPosition = 3;
            mainScaleFactor = 0.001;
            break;
        case 2:
            mainDpPosition = 2;
            mainScaleFactor = 0.01;
            break;
        }

        break;

    case PacketParser::Mode::ModeTemp:
        mainDpPosition = 1;
        break;

    case PacketParser::Mode::ModeHz:
        unitIcons |= Display::UnitIcon::mainHz;

        switch (parser.getMainScale()) {
        case 0:
            mainDpPosition = 3;
            mainScaleFactor = 0.0000001;
            break;
        case 1:
            mainDpPosition = 2;
            mainScaleFactor = 0.00000001;
            break;
        case 2:
            mainDpPosition = 1;
            mainScaleFactor = 0.000000001;
            break;
        }

        break;

    case PacketParser::Mode::ModemS:
        unitIcons |= Display::UnitIcon::mainMilliSeconds;

        switch (parser.getMainScale()) {
        case 0:
            mainDpPosition = 4;
            mainScaleFactor = 0.0000001;
            break;
        case 1:
            mainDpPosition = 3;
            mainScaleFactor = 0.000001;
            break;
        case 2:
            mainDpPosition = 2;
            mainScaleFactor = 0.00001;
            break;
        }

        break;

    case PacketParser::Mode::ModeDuty:
        unitIcons |= Display::UnitIcon::subPercent;
        mainDpPosition = 1;
        mainScaleFactor = 0.1;
        break;

    case PacketParser::Mode::ModeResistor:
        unitIcons |= Display::UnitIcon::mainOhm;

        switch (parser.getMainScale()) {
        case 0:
            mainDpPosition = 3;
            mainScaleFactor = 0.001;
            break;
        case 1:
            mainDpPosition = 2;
            mainScaleFactor = 0.01;
            break;
        case 2:
            unitIcons |= Display::UnitIcon::mainKilo;
            mainDpPosition = 4;
            mainScaleFactor = 0.1;
            break;
        case 3:
            unitIcons |= Display::UnitIcon::mainKilo;
            mainDpPosition = 3;
            mainScaleFactor = 1.0;
            break;
        case 4:
            unitIcons |= Display::UnitIcon::mainKilo;
            mainDpPosition = 2;
            mainScaleFactor = 10.0;
            break;
        case 5:
            unitIcons |= Display::UnitIcon::mainMega;
            mainDpPosition = 4;
            mainScaleFactor = 100.0;
            break;
        case 6:
            unitIcons |= Display::UnitIcon::mainMega;
            mainDpPosition = 3;
            mainScaleFactor = 1000.0;
            break;
        }

        break;

    case PacketParser::Mode::ModeContinuity:
        unitIcons |= Display::UnitIcon::mainOhm;
        icons |= Display::Icon::Beep;

        mainDpPosition = 2;
        mainScaleFactor = 0.01;
        break;

    case PacketParser::Mode::ModeDiode:
        unitIcons |= Display::UnitIcon::mainV;
        icons |= Display::Icon::Diode;

        switch (parser.getMainScale()) {
        case 0:
            mainDpPosition = 4;
            mainScaleFactor = 0.0001;
            break;
        case 1:
            mainDpPosition = 3;
            mainScaleFactor = 0.001;
            break;
        }

        break;

    case PacketParser::Mode::ModeCapacitor:
        unitIcons |= Display::UnitIcon::mainF;

        switch (parser.getMainScale()) {
        case 0:
            unitIcons |= Display::UnitIcon::mainNano;
            mainDpPosition = 2;
            mainScaleFactor = 0.00000000001;
            break;
        case 1:
            // FIXME - is this right?
            unitIcons |= Display::UnitIcon::mainNano;
            mainDpPosition = 1;
            mainScaleFactor = 0.000000000001;
            break;
        case 2:
            unitIcons |= Display::UnitIcon::mainMicro;
            mainDpPosition = 3;
            mainScaleFactor = 0.000000001;
            break;
        case 3:
            unitIcons |= Display::UnitIcon::mainMicro;
            mainDpPosition = 2;
            mainScaleFactor = 0.0000001;
            break;
        case 4:
            unitIcons |= Display::UnitIcon::mainMicro;
            mainDpPosition = 1;
            mainScaleFactor = 0.0000001;
            break;
        case 5:
            unitIcons |= Display::UnitIcon::mainMicro;
            mainDpPosition = 0;
            mainScaleFactor = 0.000000001;
            break;
        }

        break;

    //FIXME
    case PacketParser::Mode::ModeACuVA:
        break;

    //FIXME
    case PacketParser::Mode::ModeACmVA:
        break;

    //FIXME
    case PacketParser::Mode::ModeACVA:
        break;

    case PacketParser::Mode::ModeACuA:
    case PacketParser::Mode::ModeDCuA:
        unitIcons |= Display::UnitIcon::mainA1;
        unitIcons |= Display::UnitIcon::mainMicro;

        switch (parser.getMainScale()) {
        case 0:
            mainDpPosition = 3;
            mainScaleFactor = 0.0000001;
            break;
        case 1:
            mainDpPosition = 2;
            mainScaleFactor = 0.00000001;
            break;
        case 2:
            mainDpPosition = 1;
            mainScaleFactor = 0.000000001;
            break;
        }

        break;

    case PacketParser::Mode::ModeACmA:
    case PacketParser::Mode::ModeDCmA:
        unitIcons |= Display::UnitIcon::mainA1;
        unitIcons |= Display::UnitIcon::mainMilli;

        switch (parser.getMainScale()) {
        case 0:
            mainDpPosition = 4;
            mainScaleFactor = 0.0000001;
            break;
        case 1:
            mainDpPosition = 3;
            mainScaleFactor = 0.000001;
            break;
        case 2:
            mainDpPosition = 2;
            mainScaleFactor = 0.00001;
            break;
        }

        break;

    case PacketParser::Mode::ModeACA:
    case PacketParser::Mode::ModeDCA:
        unitIcons |= Display::UnitIcon::mainA1;

        switch (parser.getMainScale()) {
        // FIXME - there are possibly more range modes here
        case 1:
            mainDpPosition = 4;
            mainScaleFactor = 0.0001;
            break;
        }

        break;

    case PacketParser::Mode::ModeDCuVA:
        break;

    case PacketParser::Mode::ModeDCmVA:
        break;

    case PacketParser::Mode::ModeDCVA:
        break;

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
        QString mainValueString= QString::number(parser.getMainValue());

        while (mainDpPosition > mainValueString.length() - 1)
            mainValueString.prepend("0");

        ui->display->setMain(mainValueString, mainDpPosition);
        mainValue = (double) parser.getMainValue() * mainScaleFactor;
    }

    ui->display->setSub(QString::number(parser.getSubValue()), 0);
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

    ui->display->setIcons(icons);
    ui->display->setUnitIcons(unitIcons);
}
