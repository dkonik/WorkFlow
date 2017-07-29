#ifndef SINGLEROW_H
#define SINGLEROW_H
#include <QWidget>
#include <QtCore>
#include <list>
#include <itemwidget.h>
#include <singleitem.h>

class SingleRow
{
public:
    // Head is the first item in the row, while parent is the item in a different row
    // which this row branches from
    SingleRow(SingleItem* headIn, SingleItem* parentIn) :
        head(headIn),
        parent(parentIn)
    {
        items.push_back(headIn);
    }

    SingleItem* head;
    SingleItem* parent;
    std::list<SingleItem*> items;
};

#endif // SINGLEROW_H
