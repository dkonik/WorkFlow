#include "drawwidget.h"
#include <QDebug>
#include <QRect>
#include <QtCore>
#include <QApplication>

DrawWidget::DrawWidget(QWidget *parent) : QWidget(parent)
{

}

void DrawWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 6));
    for (ConnectedPair pair : items){
        QColor* c = assignedColors[pair.to];
        painter.setPen(QPen(*c, 6));
        const QWidget* from = pair.from;
        const QWidget* to =pair.to;

        QPoint start =  from->mapToGlobal(from->rect().topRight() +  QPoint(0, from->height()/2));
        QPoint end = to->mapToGlobal(to->rect().topLeft() +  QPoint(0, to->height()/2));

        painter.drawLine(mapFromGlobal(start), mapFromGlobal(end));
    }
}

void DrawWidget::wheelEvent(QWheelEvent *event)
{
//    if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier)){
//        scrollDelta = event->delta();
//    }
}

void DrawWidget::updateEndpoints(QWidget* startIn, QWidget* endIn, QColor* newRowColor /*= nullptr*/){
    if (newRowColor == nullptr){
        assignedColors[endIn] = assignedColors[startIn];
    } else {
        assignedColors[endIn] = newRowColor;
    }
    endIn->setStyleSheet(getStyleSheetString(assignedColors[endIn]->name()));
    items.push_back(ConnectedPair{startIn, endIn});
    update();
}
