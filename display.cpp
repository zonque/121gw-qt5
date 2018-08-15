#include <QDebug>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>

#include "display.h"

DisplaySegment::DisplaySegment(QSvgRenderer *renderer, QString elementId)
{
    setSharedRenderer(renderer);
    setElementId(elementId);
    setPos(renderer->boundsOnElement(elementId).topLeft() * 2.0);
    setScale(2.0);
    setVisible(false);
}

DisplayDigit::DisplayDigit(QGraphicsScene *scene, QSvgRenderer *renderer, QString prefix)
{
    a = new DisplaySegment(renderer, prefix + ".a");
    scene->addItem(a);

    b = new DisplaySegment(renderer, prefix + ".b");
    scene->addItem(b);

    c = new DisplaySegment(renderer, prefix + ".c");
    scene->addItem(c);

    d = new DisplaySegment(renderer, prefix + ".d");
    scene->addItem(d);

    e = new DisplaySegment(renderer, prefix + ".e");
    scene->addItem(e);

    f = new DisplaySegment(renderer, prefix + ".f");
    scene->addItem(f);

    g = new DisplaySegment(renderer, prefix + ".g");
    scene->addItem(g);

    if (renderer->elementExists(prefix + ".dp")) {
        dp = new DisplaySegment(renderer, prefix + ".dp");
        scene->addItem(dp);
    } else
        dp = Q_NULLPTR;
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

void DisplayDigit::setCharacter(char chr)
{
    setVisible(true);
    if (dp)
        dp->setVisible(false);

    switch (chr) {
    case '0':
    case 'O':
        g->setVisible(false);
        break;
    case '1':
        a->setVisible(false);
        d->setVisible(false);
        e->setVisible(false);
        f->setVisible(false);
        g->setVisible(false);
        break;
    case '2':
        c->setVisible(false);
        f->setVisible(false);
        break;
    case '3':
        e->setVisible(false);
        f->setVisible(false);
        break;
    case '4':
        a->setVisible(false);
        d->setVisible(false);
        e->setVisible(false);
        break;
    case '5':
        b->setVisible(false);
        e->setVisible(false);
        break;
    case '6':
        b->setVisible(false);
        break;
    case '7':
        d->setVisible(false);
        e->setVisible(false);
        f->setVisible(false);
        g->setVisible(false);
        break;
    case '8':
        break;
    case '9':
        e->setVisible(false);
        break;
    case 'F':
        b->setVisible(false);
        c->setVisible(false);
        d->setVisible(false);
        break;
    case 'L':
        a->setVisible(false);
        b->setVisible(false);
        c->setVisible(false);
        g->setVisible(false);
        break;
    case 'c':
        a->setVisible(false);
        b->setVisible(false);
        c->setVisible(false);
        f->setVisible(false);
        break;
    case ' ':
        setVisible(false);
        break;
    }
}

DisplayDigitGroup::DisplayDigitGroup(QGraphicsScene *scene, QSvgRenderer *renderer, QString prefix, DisplaySegment *negative)
{
    negativeSign = negative;
    scene->addItem(negativeSign);

    digits  << new DisplayDigit(scene, renderer, prefix + ".0");
    digits  << new DisplayDigit(scene, renderer, prefix + ".1");
    digits  << new DisplayDigit(scene, renderer, prefix + ".2");
    digits  << new DisplayDigit(scene, renderer, prefix + ".3");
    digits  << new DisplayDigit(scene, renderer, prefix + ".4");
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
                                        new DisplaySegment(renderer, "main.negative"));
    subDisplay = new DisplayDigitGroup(scene, renderer, "aux",
                                       new DisplaySegment(renderer, "aux.negative"));

    iconItems[Icon::Min]          = new DisplaySegment(renderer, "icon.min");
    iconItems[Icon::Max]          = new DisplaySegment(renderer, "icon.max");
    iconItems[Icon::Avg]          = new DisplaySegment(renderer, "icon.avg");
    iconItems[Icon::LowZ]         = new DisplaySegment(renderer, "icon.lowz");
    iconItems[Icon::AC]           = new DisplaySegment(renderer, "icon.ac");
    iconItems[Icon::Diode]        = new DisplaySegment(renderer, "icon.diode");
    iconItems[Icon::Beep]         = new DisplaySegment(renderer, "icon.beep");
    iconItems[Icon::LowPass]      = new DisplaySegment(renderer, "icon.lowpass");
    iconItems[Icon::BT]           = new DisplaySegment(renderer, "icon.bt");
    iconItems[Icon::DC]           = new DisplaySegment(renderer, "icon.dc");
    iconItems[Icon::Auto]         = new DisplaySegment(renderer, "icon.auto");
    iconItems[Icon::MainAC]       = new DisplaySegment(renderer, "icon.main.ac");
    iconItems[Icon::MainDC]       = new DisplaySegment(renderer, "icon.main.dc");
    iconItems[Icon::MainDCPlusAC] = new DisplaySegment(renderer, "icon.main.dc_ac");
    iconItems[Icon::Test]         = new DisplaySegment(renderer, "icon.test");
    iconItems[Icon::Mem]          = new DisplaySegment(renderer, "icon.mem");
    iconItems[Icon::AMinus]       = new DisplaySegment(renderer, "icon.a-");
    iconItems[Icon::Hold]         = new DisplaySegment(renderer, "icon.hold");
    iconItems[Icon::OneMs]        = new DisplaySegment(renderer, "icon.1ms");
    iconItems[Icon::Battery]      = new DisplaySegment(renderer, "icon.battery");
    iconItems[Icon::Apo]          = new DisplaySegment(renderer, "icon.apo");
    iconItems[Icon::Flash]        = new DisplaySegment(renderer, "icon.flash");

    for (auto key : iconItems.keys()) {
        DisplaySegment *segment = iconItems[key];
        scene->addItem(segment);
    }

    unitIconItems[UnitIcon::subdB]      = new DisplaySegment(renderer, "unit.sub.db");
    unitIconItems[UnitIcon::subMilli]       = new DisplaySegment(renderer, "unit.sub.milli");
    unitIconItems[UnitIcon::subA]       = new DisplaySegment(renderer, "unit.sub.a");
    unitIconItems[UnitIcon::subPercent] = new DisplaySegment(renderer, "unit.sub.percent");
    unitIconItems[UnitIcon::subn]       = new DisplaySegment(renderer, "unit.sub.nano");
    unitIconItems[UnitIcon::subS]       = new DisplaySegment(renderer, "unit.sub.s");
    unitIconItems[UnitIcon::subOhm]     = new DisplaySegment(renderer, "unit.sub.ohm");
    unitIconItems[UnitIcon::subV]       = new DisplaySegment(renderer, "unit.sub.v");
    unitIconItems[UnitIcon::subK]       = new DisplaySegment(renderer, "unit.sub.kilo");
    unitIconItems[UnitIcon::subHz]      = new DisplaySegment(renderer, "unit.sub.hz");

    unitIconItems[UnitIcon::mainCelcius]    = new DisplaySegment(renderer, "unit.main.celcius");
    unitIconItems[UnitIcon::mainFahrenheit] = new DisplaySegment(renderer, "unit.main.fahrenheit");
    unitIconItems[UnitIcon::mainMilli]      = new DisplaySegment(renderer, "unit.main.milli");
    unitIconItems[UnitIcon::mainV]          = new DisplaySegment(renderer, "unit.main.v");
    unitIconItems[UnitIcon::mainA1]         = new DisplaySegment(renderer, "unit.main.a1");
    unitIconItems[UnitIcon::mainMicro]      = new DisplaySegment(renderer, "unit.main.micro");
    unitIconItems[UnitIcon::mainNano]       = new DisplaySegment(renderer, "unit.main.nano");
    unitIconItems[UnitIcon::mainF]          = new DisplaySegment(renderer, "unit.main.f");
    unitIconItems[UnitIcon::mainA2]         = new DisplaySegment(renderer, "unit.main.a2");
    unitIconItems[UnitIcon::mainMega]       = new DisplaySegment(renderer, "unit.main.mega");
    unitIconItems[UnitIcon::mainKilo]       = new DisplaySegment(renderer, "unit.main.kilo");
    unitIconItems[UnitIcon::mainOhm]        = new DisplaySegment(renderer, "unit.main.ohm");
    unitIconItems[UnitIcon::mainHz]         = new DisplaySegment(renderer, "unit.main.hz");

    for (auto key : unitIconItems.keys()) {
        DisplaySegment *segment = unitIconItems[key];
        scene->addItem(segment);
    }

    for (int i = 0; i < 26; i++) {
        DisplaySegment *segment = new DisplaySegment(renderer, QString::asprintf("bar.%d", i));
        barItems << segment;
        scene->addItem(segment);
    }

    scale = new DisplaySegment(renderer, "scale");
    scene->addItem(scale);

    scale_500 = new DisplaySegment(renderer, "scale.500");
    scene->addItem(scale_500);

    scale_1000 = new DisplaySegment(renderer, "scale.1000");
    scene->addItem(scale_1000);

    barNegative = new DisplaySegment(renderer, "bar.negative");
    scene->addItem(barNegative);

    barPositive = new DisplaySegment(renderer, "bar.positive");
    scene->addItem(barPositive);
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
    scale_500->setVisible(false);
    scale_1000->setVisible(false);

    switch (status) {
    case Off:
        barNegative->setVisible(false);
        barPositive->setVisible(false);

        for (int i = 0; i < barItems.length(); i++)
            barItems.at(i)->setVisible(false);

        break;

    case On500:
        scale->setVisible(true);
        scale_500->setVisible(true);
        break;

    case On1000:
        scale->setVisible(true);
        scale_1000->setVisible(true);
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
