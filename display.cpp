#include <QDebug>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>

#include "display.h"

DisplaySegment::DisplaySegment(QGraphicsScene *scene, QSvgRenderer *renderer, QString elementId)
{
    setSharedRenderer(renderer);
    setElementId(elementId);
    setPos(renderer->boundsOnElement(elementId).topLeft() * 2.0);
    setScale(2.0);
    setVisible(false);
    scene->addItem(this);
}

DisplayDigit::DisplayDigit(QGraphicsScene *scene, QSvgRenderer *renderer, QString prefix)
{
    a = new DisplaySegment(scene, renderer, prefix + ".a");
    b = new DisplaySegment(scene, renderer, prefix + ".b");
    c = new DisplaySegment(scene, renderer, prefix + ".c");
    d = new DisplaySegment(scene, renderer, prefix + ".d");
    e = new DisplaySegment(scene, renderer, prefix + ".e");
    f = new DisplaySegment(scene, renderer, prefix + ".f");
    g = new DisplaySegment(scene, renderer, prefix + ".g");

    if (renderer->elementExists(prefix + ".dp"))
        dp = new DisplaySegment(scene, renderer, prefix + ".dp");
    else
        dp = Q_NULLPTR;
}

DisplayDigit::~DisplayDigit()
{
    delete a;
    delete b;
    delete c;
    delete d;
    delete e;
    delete f;
    delete g;

    if (dp)
        delete dp;
}

void DisplayDigit::setVisible(bool visible)
{
    a->setVisible(visible);
    b->setVisible(visible);
    c->setVisible(visible);
    d->setVisible(visible);
    e->setVisible(visible);
    f->setVisible(visible);
    g->setVisible(visible);

    if (dp)
        dp->setVisible(visible);
}

void DisplayDigit::setDpVisible(bool visible)
{
    if (dp)
        dp->setVisible(visible);
}

enum {
    SEG_A = 1 << 0,
    SEG_B = 1 << 1,
    SEG_C = 1 << 2,
    SEG_D = 1 << 3,
    SEG_E = 1 << 4,
    SEG_F = 1 << 5,
    SEG_G = 1 << 6,
};

uint8_t DisplayDigit::getCharacterSegments(char chr)
{
    switch (chr) {
    case '0':
    case 'O':
        return SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
    case '1':
        return SEG_B | SEG_C;
    case '2':
        return SEG_A | SEG_B | SEG_D | SEG_E | SEG_G;
    case '3':
        return SEG_A | SEG_B | SEG_C | SEG_D | SEG_G;
    case '4':
        return SEG_B | SEG_C | SEG_F | SEG_G;
    case '5':
    case 'S':
        return SEG_A | SEG_C | SEG_D | SEG_F | SEG_G;
    case '6':
        return SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
    case '7':
        return SEG_A | SEG_B | SEG_C;
    case '8':
        return SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
    case 'g':
    case '9':
        return SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;
    case 'A':
        return SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;
    case 'b':
        return SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
    case 'C':
        return SEG_A | SEG_D | SEG_E | SEG_F;
    case 'c':
        return SEG_D | SEG_E | SEG_G;
    case 'd':
        return SEG_B | SEG_C | SEG_D | SEG_E | SEG_G;
    case 'E':
        return SEG_A | SEG_D | SEG_E | SEG_F | SEG_G;
    case 'F':
        return SEG_A | SEG_E | SEG_F | SEG_G;
    case 'h':
        return SEG_C | SEG_E | SEG_F | SEG_G;
    case 'i':
        return SEG_C;
    case 'I':
        return SEG_B | SEG_C;
    case 'J':
    case 'j':
        return SEG_B | SEG_C | SEG_D;
    case 'L':
        return SEG_D | SEG_E | SEG_F;
    case 'N':
        return SEG_A | SEG_B | SEG_C | SEG_E | SEG_F;
    case 'n':
        return SEG_C | SEG_E | SEG_G;
    case 'o':
        return SEG_C | SEG_D | SEG_E | SEG_G;
    case 'P':
    case 'p':
        return SEG_A | SEG_B | SEG_E | SEG_F | SEG_G;
    case 'r':
        return SEG_E | SEG_G;
    case 't':
        return SEG_D | SEG_E | SEG_F | SEG_G;
    case 'U':
        return SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
    case 'u':
        return SEG_C | SEG_D | SEG_E;
    case 'y':
        return SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;
    case '-':
        return SEG_G;
    case '_':
        return SEG_D;
    case ' ':
    default:
        return 0;
    }

    return 0;
}

void DisplayDigit::setCharacter(char chr)
{
    uint8_t segments = getCharacterSegments(chr);

    a->setVisible(!!(segments & SEG_A));
    b->setVisible(!!(segments & SEG_B));
    c->setVisible(!!(segments & SEG_C));
    d->setVisible(!!(segments & SEG_D));
    e->setVisible(!!(segments & SEG_E));
    f->setVisible(!!(segments & SEG_F));
    g->setVisible(!!(segments & SEG_G));

    if (dp)
        dp->setVisible(false);
}

DisplayDigitGroup::DisplayDigitGroup(QGraphicsScene *scene, QSvgRenderer *renderer, QString prefix, DisplaySegment *negative)
{
    negativeSign = negative;

    digits << new DisplayDigit(scene, renderer, prefix + ".0");
    digits << new DisplayDigit(scene, renderer, prefix + ".1");
    digits << new DisplayDigit(scene, renderer, prefix + ".2");
    digits << new DisplayDigit(scene, renderer, prefix + ".3");
    digits << new DisplayDigit(scene, renderer, prefix + ".4");
}

DisplayDigitGroup::~DisplayDigitGroup()
{
    while (!digits.empty())
        delete digits.takeFirst();
}

void DisplayDigitGroup::setString(const QString &string)
{
    if (string.length() > digits.length())
        return;

    QString padded = string.rightJustified(digits.length(), ' ');

    for (int i = 0; i < digits.length(); i++)
        digits[i]->setCharacter(padded.at(padded.length() - i -1).toLatin1());
}

void DisplayDigitGroup::setDpPosition(int position)
{
    for (int i = 0; i < digits.length(); i++)
        digits[i]->setDpVisible(position == i);
}

Display::Display(QWidget *parent) : QGraphicsView(parent)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    setScene(scene);

    renderer = new QSvgRenderer(QLatin1String(":/assets/121gw-lcd.svg"));

    mainDisplay = new DisplayDigitGroup(scene, renderer, "main",
                                        new DisplaySegment(scene, renderer, "main.negative"));
    subDisplay = new DisplayDigitGroup(scene, renderer, "aux",
                                       new DisplaySegment(scene, renderer, "aux.negative"));

    iconItems[Icon::Min]          = new DisplaySegment(scene, renderer, "icon.min");
    iconItems[Icon::Max]          = new DisplaySegment(scene, renderer, "icon.max");
    iconItems[Icon::Avg]          = new DisplaySegment(scene, renderer, "icon.avg");
    iconItems[Icon::LowZ]         = new DisplaySegment(scene, renderer, "icon.lowz");
    iconItems[Icon::AC]           = new DisplaySegment(scene, renderer, "icon.ac");
    iconItems[Icon::Diode]        = new DisplaySegment(scene, renderer, "icon.diode");
    iconItems[Icon::Beep]         = new DisplaySegment(scene, renderer, "icon.beep");
    iconItems[Icon::LowPass]      = new DisplaySegment(scene, renderer, "icon.lowpass");
    iconItems[Icon::BT]           = new DisplaySegment(scene, renderer, "icon.bt");
    iconItems[Icon::DC]           = new DisplaySegment(scene, renderer, "icon.dc");
    iconItems[Icon::Auto]         = new DisplaySegment(scene, renderer, "icon.auto");
    iconItems[Icon::MainAC]       = new DisplaySegment(scene, renderer, "icon.main.ac");
    iconItems[Icon::MainDC]       = new DisplaySegment(scene, renderer, "icon.main.dc");
    iconItems[Icon::MainDCPlusAC] = new DisplaySegment(scene, renderer, "icon.main.dc_ac");
    iconItems[Icon::Test]         = new DisplaySegment(scene, renderer, "icon.test");
    iconItems[Icon::Mem]          = new DisplaySegment(scene, renderer, "icon.mem");
    iconItems[Icon::AMinus]       = new DisplaySegment(scene, renderer, "icon.a-");
    iconItems[Icon::Hold]         = new DisplaySegment(scene, renderer, "icon.hold");
    iconItems[Icon::OneMs]        = new DisplaySegment(scene, renderer, "icon.1ms");
    iconItems[Icon::Battery]      = new DisplaySegment(scene, renderer, "icon.battery");
    iconItems[Icon::Apo]          = new DisplaySegment(scene, renderer, "icon.apo");
    iconItems[Icon::Flash]        = new DisplaySegment(scene, renderer, "icon.flash");

    unitIconItems[UnitIcon::subdB]          = new DisplaySegment(scene, renderer, "unit.sub.db");
    unitIconItems[UnitIcon::subMilli]       = new DisplaySegment(scene, renderer, "unit.sub.milli");
    unitIconItems[UnitIcon::subA]           = new DisplaySegment(scene, renderer, "unit.sub.a");
    unitIconItems[UnitIcon::subPercent]     = new DisplaySegment(scene, renderer, "unit.sub.percent");
    unitIconItems[UnitIcon::subn]           = new DisplaySegment(scene, renderer, "unit.sub.nano");
    unitIconItems[UnitIcon::subS]           = new DisplaySegment(scene, renderer, "unit.sub.s");
    unitIconItems[UnitIcon::subOhm]         = new DisplaySegment(scene, renderer, "unit.sub.ohm");
    unitIconItems[UnitIcon::subV]           = new DisplaySegment(scene, renderer, "unit.sub.v");
    unitIconItems[UnitIcon::subK]           = new DisplaySegment(scene, renderer, "unit.sub.kilo");
    unitIconItems[UnitIcon::subHz]          = new DisplaySegment(scene, renderer, "unit.sub.hz");
    unitIconItems[UnitIcon::mainCelcius]    = new DisplaySegment(scene, renderer, "unit.main.celcius");
    unitIconItems[UnitIcon::mainFahrenheit] = new DisplaySegment(scene, renderer, "unit.main.fahrenheit");
    unitIconItems[UnitIcon::mainMilli]      = new DisplaySegment(scene, renderer, "unit.main.milli");
    unitIconItems[UnitIcon::mainV]          = new DisplaySegment(scene, renderer, "unit.main.v");
    unitIconItems[UnitIcon::mainA1]         = new DisplaySegment(scene, renderer, "unit.main.a1");
    unitIconItems[UnitIcon::mainMicro]      = new DisplaySegment(scene, renderer, "unit.main.micro");
    unitIconItems[UnitIcon::mainNano]       = new DisplaySegment(scene, renderer, "unit.main.nano");
    unitIconItems[UnitIcon::mainF]          = new DisplaySegment(scene, renderer, "unit.main.f");
    unitIconItems[UnitIcon::mainA2]         = new DisplaySegment(scene, renderer, "unit.main.a2");
    unitIconItems[UnitIcon::mainMega]       = new DisplaySegment(scene, renderer, "unit.main.mega");
    unitIconItems[UnitIcon::mainKilo]       = new DisplaySegment(scene, renderer, "unit.main.kilo");
    unitIconItems[UnitIcon::mainOhm]        = new DisplaySegment(scene, renderer, "unit.main.ohm");
    unitIconItems[UnitIcon::mainHz]         = new DisplaySegment(scene, renderer, "unit.main.hz");

    for (int i = 0; i < 26; i++)
        barItems << new DisplaySegment(scene, renderer, QString::asprintf("bar.%d", i));

    scale = new DisplaySegment(scene, renderer, "scale");
    scale_5 = new DisplaySegment(scene, renderer, "scale.5");
    scale_50 = new DisplaySegment(scene, renderer, "scale.50");
    scale_500 = new DisplaySegment(scene, renderer, "scale.500");
    scale_10 = new DisplaySegment(scene, renderer, "scale.10");
    scale_100 = new DisplaySegment(scene, renderer, "scale.100");
    scale_1000 = new DisplaySegment(scene, renderer, "scale.1000");
    barNegative = new DisplaySegment(scene, renderer, "bar.negative");
    barPositive = new DisplaySegment(scene, renderer, "bar.positive");
}

Display::~Display()
{
    while (!barItems.empty())
        delete barItems.takeFirst();

    for (auto key : iconItems.keys())
        delete iconItems[key];

    for (auto key : unitIconItems.keys())
        delete unitIconItems[key];

    delete scale;
    delete scale_5;
    delete scale_50;
    delete scale_500;
    delete scale_10;
    delete scale_100;
    delete scale_1000;
    delete barNegative;
    delete barPositive;

    delete subDisplay;
    delete mainDisplay;
    delete renderer;
}

void Display::setMain(const QString &string, int dpPosition)
{
    mainDisplay->setString(string);
    mainDisplay->setDpPosition(dpPosition);
}

void Display::setSub(const QString &string, int dpPosition)
{
    subDisplay->setString(string);
    subDisplay->setDpPosition(dpPosition);
}

void Display::setIcons(Icons icons)
{
    for (auto key : iconItems.keys()) {
        DisplaySegment *segment = iconItems[key];
        segment->setVisible(icons & key);
    }

    update();
}

void Display::setUnitIcons(UnitIcons icons)
{
    for (auto key : unitIconItems.keys()) {
        DisplaySegment *segment = unitIconItems[key];
        segment->setVisible(icons & key);
    }

    update();
}

void Display::setBarStatus(Display::BarStatus status)
{
    scale->setVisible(false);
    scale_5->setVisible(false);
    scale_50->setVisible(false);
    scale_500->setVisible(false);
    scale_10->setVisible(false);
    scale_100->setVisible(false);
    scale_1000->setVisible(false);

    switch (status) {
    case Off:
        barNegative->setVisible(false);
        barPositive->setVisible(false);

        for (int i = 0; i < barItems.length(); i++)
            barItems.at(i)->setVisible(false);

        break;

    case On500:
        scale_500->setVisible(true);
        // fall-through
    case On50:
        scale_50->setVisible(true);
        // fall-through
    case On5:
        scale_5->setVisible(true);
        scale->setVisible(true);
        break;

    case On1000:
        scale_1000->setVisible(true);
        // fall-through
    case On100:
        scale_100->setVisible(true);
        // fall-through
    case On10:
        scale_10->setVisible(true);
        scale->setVisible(true);
        break;
    }

    update();
}

void Display::setBarValue(uint8_t barValue, bool negative)
{
    for (int i = 0; i < barItems.length(); i++)
        barItems.at(i)->setVisible(barValue >= i);

    barNegative->setVisible(false);
    barPositive->setVisible(false);

    if (barValue) {
        if (negative)
            barNegative->setVisible(true);
        else
            barPositive->setVisible(true);
    }

    update();
}
