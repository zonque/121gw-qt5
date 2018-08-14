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

void DisplayDigit::setCharacter(char chr)
{
    setVisible(true);

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
        break;
    case 'c':
        a->setVisible(false);
        b->setVisible(false);
        c->setVisible(false);
        e->setVisible(false);
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
}

Display::Display(QWidget *parent) : QGraphicsView(parent)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    setScene(scene);

    renderer = new QSvgRenderer(QLatin1String(":/assets/121gw-lcd.svg"));

    mainDisplay = new DisplayDigitGroup(scene, renderer, "main",
                                        new DisplaySegment(renderer, "main.negative"));
    auxDisplay = new DisplayDigitGroup(scene, renderer, "aux",
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

    show();
}

void Display::setIcons(Icons icons)
{
    for (auto key : iconItems.keys()) {
        DisplaySegment *segment = iconItems[key];
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

void Display::setBarValue(uint8_t barValue)
{
    for (int i = 0; i < barItems.length(); i++)
        barItems.at(i)->setVisible(barValue >= i);

    update();
}

void Display::setBarNegative(bool negative)
{
    if (negative) {
        barNegative->setVisible(true);
        barPositive->setVisible(false);
    } else {
        barNegative->setVisible(false);
        barPositive->setVisible(true);
    }

    update();
}
