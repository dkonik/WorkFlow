#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <itemwidget.h>
#include <singlerow.h>

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
    singleRow->indexInRowsList = rows.size();
    rows.push_back(singleRow);

    QApplication::connect(scrollArea->horizontalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(scrollHorizontal()));
    QApplication::connect(scrollArea->verticalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(scrollHorizontal()));
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
    QPushButton* button2 = new QPushButton("Fork");

    QApplication::connect(button1, SIGNAL(clicked(bool)), this, SLOT(AddItemMiddle()));
    QApplication::connect(button2, SIGNAL(clicked(bool)), this, SLOT(AddItemDown()));


    rightSidePanel->addWidget(button1);
    rightSidePanel->addWidget(button2);


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

        w->setLocation(
                    parentItemWidget->getColumn() + 1,
                    (parentItemWidget->getRow() >= shiftDownIfBelowThis) ? parentItemWidget->getRow() + 1 : parentItemWidget->getRow());

        row->items.push_back(singleItem);
        gridLayout->addWidget(w,w->getRow(), w->getColumn());
        drawWidget->updateEndpoints(parentItemWidget, w);
        ++i;
    }
}

void MainWindow::scrollHorizontal()
{
    if (mostRecentlyAdded != nullptr){
        scrollArea->ensureWidgetVisible(mostRecentlyAdded);
        drawWidget->update();
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
        //w->setStyleSheet(getStyleSheetString(drawWidget->assignedColors[w]->name()));

        SingleItem* singleItem = new SingleItem(w);
        w->container = singleItem;

        // Since this is middle, the parent is the same
        SingleRow* parentsRow = parent->container->rowContainer;
        singleItem->rowContainer = parentsRow;
        parentsRow->items.push_back(singleItem);

        w->setLocation(parent->getColumn() + 1, parent->getRow());

        // Didn't add new row so don't need to shift down
        shiftDownIfBelowThis = 100000; // Horrible style, I know
        modifiedRow = parentsRow->indexInRowsList;
    }
        break;
    case CreationDirection::Down:
    {
        drawWidget->updateEndpoints(parent, w, new QColor(colors[colorIndex++ % colors.size()]));
        //w->setStyleSheet(getStyleSheetString(drawWidget->assignedColors[w]->name()));

        SingleItem* singleItem = new SingleItem(w);
        w->container = singleItem;

        SingleRow* newRow = new SingleRow(singleItem, parent->container);
        singleItem->rowContainer = newRow;

        newRow->indexInRowsList = rows.size();
        rows.push_back(newRow);

        w->setLocation(parent->getColumn() + 1, parent->getRow() + 1);
        shiftDownIfBelowThis = w->getRow();
        modifiedRow = rows.size() - 1;
    }
        break;
    }

    gridLayout->addWidget(w,w->getRow(), w->getColumn());
    mostRecentlyAdded = w;
    AddNewColumn();
}
