#ifndef SINGLEITEM_H
#define SINGLEITEM_H

class ItemWidget;
class SingleRow;

enum CreationDirection{
    Middle, Down
};

class SingleItem
{
public:
    SingleItem(ItemWidget* in) : item(in)
    { }

    bool hasBranch = false;
    CreationDirection directionOfBranch;
    ItemWidget* item;
    SingleRow* rowContainer;
};

#endif // SINGLEITEM_H
