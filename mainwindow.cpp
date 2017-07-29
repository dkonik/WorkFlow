#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <itemwidget.h>
#include <singlerow.h>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scrollArea = new QScrollArea();
    setCentralWidget(scrollArea);

    drawWidget = new DrawWidget();
    gridLayout = new QGridLayout();
    gridLayout->setVerticalSpacing(100);
    gridLayout->setHorizontalSpacing(300);
    drawWidget->setLayout(gridLayout);

    scrollArea->setWidget(drawWidget);
    scrollArea->setWidgetResizable(true);

    ItemWidget* w = CreateNewItem();
    w->setLocation(startCol, startRow);
    gridLayout->addWidget(w,startRow, startCol++);
    drawWidget->assignedColors[w] = new QColor(colors[colorIndex++]);
    w->setStyleSheet(getStyleSheetString(drawWidget->assignedColors[w]->name()));

    SingleItem* singleItem = new SingleItem(w);
    w->container = singleItem;
    SingleRow* singleRow = new SingleRow(singleItem, nullptr);
    singleItem->rowContainer = singleRow;
    rows.push_back(singleRow);

    QApplication::connect(scrollArea->horizontalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(scrollHorizontal()));
    QApplication::connect(scrollArea->verticalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(scrollHorizontal()));

    // This is how to add a menu bar
    QAction* newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));

    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
}

// This is just creating a single one of the example widgets which I want to connect
ItemWidget* MainWindow::CreateNewItem(QString text/* = ""*/){
    ItemWidget* itemWidget = new ItemWidget();

    // This fuckery is so that the children don't inherit the parent's style sheet
    QString settingStyle = QString(" ItemWidget {background-color:") +
            QString(" #FFFFFF;border-radius: 10px;}");
    itemWidget->setStyleSheet(settingStyle);
    QHBoxLayout* singleItemLayout = new QHBoxLayout();
    itemWidget->setLayout(singleItemLayout);

    QTextEdit* textEdit;
    if (text == "")
    {
        textEdit = new QTextEdit(std::to_string(counter++).c_str());
    }
    else
    {
        textEdit = new QTextEdit(text);
    }

    QApplication::connect(textEdit, SIGNAL(textChanged()), itemWidget, SLOT(updateTextBoxString()));

    itemWidget->textBoxString = textEdit->toPlainText();
    textEdit->setStyleSheet("background-color:white;");
    singleItemLayout->addWidget(textEdit);

    QVBoxLayout* rightSidePanel = new QVBoxLayout();
    rightSidePanel->setAlignment(Qt::AlignTop);

    QPushButton* button1 = new QPushButton("Update");
    QPushButton* button2 = new QPushButton("Branch");
    QPushButton* button3 = new QPushButton("Complete");

    QApplication::connect(button1, SIGNAL(clicked(bool)), this, SLOT(AddItemMiddle()));
    QApplication::connect(button2, SIGNAL(clicked(bool)), this, SLOT(AddItemDown()));
    QApplication::connect(button3, SIGNAL(clicked(bool)), this, SLOT(CompleteRow()));


    rightSidePanel->addWidget(button1);
    rightSidePanel->addWidget(button2);
    rightSidePanel->addWidget(button3);


    QWidget* rightPanelWidget = new QWidget();
    rightSidePanel->setMargin(0);
    rightPanelWidget->setLayout(rightSidePanel);

    singleItemLayout->addWidget(rightPanelWidget);

    itemWidget->setLayout(singleItemLayout);
    itemWidget->setMinimumWidth(300);
    itemWidget->setFixedSize(300,150);

    return itemWidget;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::AddNewColumn()
{
    int i = 0;
    for (SingleRow* row : rows){
        // Skip the modified row because we already added new item in it
        if (i == modifiedRow){
            ++i;
            continue;
        }

        ItemWidget* parentItemWidget = row->items.back()->item;
        ItemWidget* w = CreateNewItem(parentItemWidget->textBoxString);
        SingleItem* singleItem = new SingleItem(w);
        w->container = singleItem;
        singleItem->rowContainer = row;

        int newRow;
        if (parentItemWidget->getRow() >= shiftDownIfBelowThis){
            newRow = parentItemWidget->getRow() + 1;
        }
        else if (parentItemWidget->getRow() >= shiftUpIfBelowThis) {
            newRow = parentItemWidget->getRow() - 1;
        }
        else{
            newRow = parentItemWidget->getRow();
        }
        w->setLocation(parentItemWidget->getColumn() + 1, newRow);

        row->items.push_back(singleItem);
        gridLayout->addWidget(w,w->getRow(), w->getColumn());
        drawWidget->updateEndpoints(parentItemWidget, w);

        if (mostRecentlyAdded == nullptr){
            // This happens when you hit "complete". A new row is added,
            // but no item was added so we just put a dummy one
            mostRecentlyAdded = w;
        }

        ++i;
    }
}

void MainWindow::scrollHorizontal()
{
    if (mostRecentlyAdded != nullptr){
        scrollArea->ensureWidgetVisible(mostRecentlyAdded);
        drawWidget->update();
        mostRecentlyAdded = nullptr;
    }
}

void MainWindow::AddItemMiddle()
{
    AddItem(CreationDirection::Middle, QObject::sender());
}

void MainWindow::AddItemDown()
{
    AddItem(CreationDirection::Down, QObject::sender());
}

void MainWindow::CompleteRow()
{
    if (rows.size() == 1){
        // This is the only task, so do nothing
        return;
    }

    ItemWidget* sender = qobject_cast<ItemWidget*>(QObject::sender()->parent()->parent());
    SingleRow* row = sender->container->rowContainer;
    rows.remove(row);
    mostRecentlyAdded = nullptr;
    modifiedRow = -5;
    shiftUpIfBelowThis = sender->getRow(); shiftDownIfBelowThis = 100000;
    AddNewColumn();
}

int MainWindow::positionInListHelper(SingleRow* rowToFind){
    int i = 0;
    for(SingleRow* row : rows){
        if (row == rowToFind){
            return i;
        }
        ++i;
    }

    // If we get here...fuck
    Q_ASSERT(false);
    return 0;
}

void MainWindow::AddItem(CreationDirection direction, QObject* sender)
{
    ItemWidget* parent = qobject_cast<ItemWidget*>(sender->parent()->parent());
    ItemWidget* w = CreateNewItem(parent->textBoxString);
    if (parent == nullptr){
        qDebug() << "Parent was nullptr in AddItem";
        return;
    }
    w->setItemParent(parent);

    switch (direction){
    case CreationDirection::Middle:
    {
        drawWidget->updateEndpoints(parent, w);

        SingleItem* singleItem = new SingleItem(w);
        w->container = singleItem;

        // Since this is middle, the parent is the same
        SingleRow* parentsRow = parent->container->rowContainer;
        singleItem->rowContainer = parentsRow;
        parentsRow->items.push_back(singleItem);

        w->setLocation(parent->getColumn() + 1, parent->getRow());

        // Didn't add new row so don't need to shift down
        shiftDownIfBelowThis = 100000; shiftUpIfBelowThis = 100000;// Horrible style, I know
        modifiedRow = positionInListHelper(parentsRow);
    }
        break;
    case CreationDirection::Down:
    {
        drawWidget->updateEndpoints(parent, w, new QColor(colors[colorIndex++ % colors.size()]));

        SingleItem* singleItem = new SingleItem(w);
        w->container = singleItem;

        SingleRow* newRow = new SingleRow(singleItem, parent->container);
        singleItem->rowContainer = newRow;

        rows.push_back(newRow);

        w->setLocation(parent->getColumn() + 1, parent->getRow() + 1);
        shiftDownIfBelowThis = w->getRow(); shiftUpIfBelowThis = 100000;
        modifiedRow = rows.size() - 1;
    }
        break;
    }

    gridLayout->addWidget(w,w->getRow(), w->getColumn());
    mostRecentlyAdded = w;
    AddNewColumn();
}
