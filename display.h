#pragma once

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsSvgItem>

class QGraphicsSvgItem;
class QSvgRenderer;

class DisplaySegment : public QGraphicsSvgItem
{
    Q_OBJECT
public:
    explicit DisplaySegment(QSvgRenderer *renderer, QString elementId);
};

class DisplayDigit : public QObject
{
    Q_OBJECT
public:
    explicit DisplayDigit(QGraphicsScene *scene, QSvgRenderer *renderer, QString prefix);
    void setVisible(bool visible);
    void setCharacter(char chr);
private:
    DisplaySegment *a, *b, *c, *d, *e, *f, *g, *dp;
};

class DisplayDigitGroup : public QObject
{
    Q_OBJECT
public:
    explicit DisplayDigitGroup(QGraphicsScene *scene, QSvgRenderer *renderer, QString prefix, DisplaySegment *negative);

private:
    DisplaySegment *negativeSign;
    QList<DisplayDigit*> digits;
};

class Display : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Display(QWidget *parent = nullptr);

    enum Icon {
        Min           = 1 << 0,
        Max           = 1 << 1,
        Avg           = 1 << 2,
        LowZ          = 1 << 3,
        AC            = 1 << 4,
        Diode         = 1 << 5,
        Beep          = 1 << 6,
        LowPass       = 1 << 7,
        BT            = 1 << 8,
        DC            = 1 << 9,
        Auto          = 1 << 10,
        MainAC        = 1 << 11,
        MainDC        = 1 << 11,
        MainDCPlusAC  = 1 << 11,
        Test          = 1 << 12,
        Mem           = 1 << 13,
        AMinus        = 1 << 14,
        Hold          = 1 << 15,
        OneMs         = 1 << 16,
        Battery       = 1 << 17,
        Apo           = 1 << 18,
        Flash         = 1 << 19,
        Rel           = 1 << 20,
    };
    Q_DECLARE_FLAGS(Icons, Icon);

    const int numIcons = 21;

    void setIcons(Icons icons);
    void setBarValue(uint8_t barValue);

private:
    QSvgRenderer* renderer;
    DisplayDigitGroup *mainDisplay, *auxDisplay;
    QMap<enum Icon, DisplaySegment*> iconItems;
    QList<DisplaySegment *> barItems;
    DisplaySegment *scale;
};

