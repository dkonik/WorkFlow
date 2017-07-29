#include "itemwidget.h"
#include <QStyleOption>
#include <QPainter>
#include <QTextEdit>
#include <QString>

ItemWidget::ItemWidget(QWidget *parent) : QWidget(parent)
{

}

void ItemWidget::setLocation(int colIn, int rowIn)
{
    column = colIn; row = rowIn;
}

void ItemWidget::paintEvent(QPaintEvent *pe)
{
  QStyleOption o;
  o.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(
              QStyle::PE_Widget, &o, &p, this);
}

void ItemWidget::updateTextBoxString()
{
    textBoxString = qobject_cast<QTextEdit*>(QObject::sender())->toPlainText();
}
