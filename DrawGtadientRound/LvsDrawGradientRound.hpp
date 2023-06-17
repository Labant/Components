#ifndef LVSDRAWGRADIENTROUND_HPP
#define LVSDRAWGRADIENTROUND_HPP
#include <QObject>
#include <QPainter>
#include <QBrush>
#include <QImage>
#include <QRadialGradient>

class LvsDrawGradientRound:QObject
{

private:
    int radius = 120;

public:
    explicit LvsDrawGradientRound(){};
    ~LvsDrawGradientRound(){};

    void drawGradientRound(QImage& src,QPointF pos)
    {

    QPainter mPainter(&src);
    mPainter.setRenderHint(QPainter::Antialiasing,true);

    QRadialGradient mRadialGradient(pos.rx(),pos.ry(),this->radius,pos.rx(),pos.ry());
    mRadialGradient.setColorAt(0,Qt::red);
    mRadialGradient.setColorAt(0.5,Qt::blue);
    mRadialGradient.setColorAt(1,Qt::green);

    mPainter.setPen(Qt::NoPen);
    mPainter.setBrush(QBrush(mRadialGradient));
    mPainter.drawEllipse(pos.rx() - this->radius,pos.ry() - this->radius,this->radius << 1,this->radius << 1);

    };

 //!setting
    void setRadius(int R)
    {
        this->radius = R;
    };
};



#endif
