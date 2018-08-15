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
    void setDpVisible(bool visible);
    void setCharacter(char chr);
private:
    DisplaySegment *a, *b, *c, *d, *e, *f, *g, *dp;
};

class DisplayDigitGroup : public QObject
{
    Q_OBJECT
public:
    explicit DisplayDigitGroup(QGraphicsScene *scene, QSvgRenderer *renderer, QString prefix, DisplaySegment *negative);
    void setString(const QString &string);
    void setDpPosition(int position);
private:
    DisplaySegment *negativeSign;
    QList<DisplayDigit*> digits;
};

class Display : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Display(QWidget *parent = nullptr);

    enum BarStatus {
        Off,
        On500,
        On1000,
    };

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

    enum UnitIcon {
        subdB             = 1 << 0,
        subm              = 1 << 1,
        subA              = 1 << 2,
        subPercent        = 1 << 3,
        subn              = 1 << 4,
        subS              = 1 << 5,
        subOhm            = 1 << 6,
        subV              = 1 << 7,
        subK              = 1 << 8,
        subHz             = 1 << 9,
        mainCelcius       = 1 << 10,
        mainFahrenheit    = 1 << 11,
        mainHz            = 1 << 12,
    };
    Q_DECLARE_FLAGS(UnitIcons, UnitIcon);

    void setMain(const QString &string, int dpPosition);
    void setSub(const QString &string, int dpPosition);

    void setIcons(Icons icons);
    void setUnitIcons(UnitIcons icons);
    void setBarStatus(BarStatus status);
    void setBarValue(uint8_t barValue);
    void setBarNegative(bool negative);

private:
    QSvgRenderer* renderer;
    DisplayDigitGroup *mainDisplay, *subDisplay;
    QMap<enum Icon, DisplaySegment*> iconItems;
    QMap<enum UnitIcon, DisplaySegment*> unitIconItems;
    QList<DisplaySegment *> barItems;
    DisplaySegment *scale, *scale_500, *scale_1000;
    DisplaySegment *barNegative, *barPositive;
};

