#include "drawscrollarea.h"
#include <QDebug>

DrawScrollArea::DrawScrollArea()
{

}

void DrawScrollArea::wheelEvent(QWheelEvent *ev)
{
    zoomAmount = ev->delta();
    this->viewport()->update();
}

void DrawScrollArea::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.scale(zoomAmount, zoomAmount);
    zoomAmount = 0;
    qDebug() << "here";
}
