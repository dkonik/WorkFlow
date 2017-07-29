#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include <QWidget>
#include <singleitem.h>
#include <QString>

class ItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ItemWidget(QWidget *parent = nullptr);
    void setLocation(int colIn, int rowIn);
    void paintEvent(QPaintEvent *pe);
    int getColumn(){ return column;}
    int getRow() {return row;}
    void setItemParent(ItemWidget* parentIn){ parent = parentIn;}
    ItemWidget* getItemParent() {return parent;}
    SingleItem* container;
    QString textBoxString;
signals:

public slots:
    void updateTextBoxString();

private:
    int column, row;
    ItemWidget* parent;
};

#endif // ITEMWIDGET_H
