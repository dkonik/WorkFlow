#ifndef DRAWSCROLLAREA_H
#define DRAWSCROLLAREA_H

#include <QWidget>
#include <QScrollArea>
#include <QPainter>
#include <QtCore>
#include <QPen>
#include <QWheelEvent>

class DrawScrollArea : public QScrollArea
{
public:
    DrawScrollArea();
    virtual void wheelEvent(QWheelEvent* ev);
    virtual void paintEvent(QPaintEvent*);

private:
    int zoomAmount = 0;
};

#endif // DRAWSCROLLAREA_H
