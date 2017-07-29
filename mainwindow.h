#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGridLayout>
#include <QWidget>
#include <QMainWindow>
#include <QScrollArea>
#include <QWheelEvent>
#include <drawwidget.h>
#include "drawscrollarea.h"
#include <vector>
#include "itemwidget.h"
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QScrollArea* scrollArea;
    DrawWidget* drawWidget;
    QGridLayout* gridLayout;
    std::list<SingleRow*> rows;

protected:
    void AddNewColumn();

public slots:
    void scrollHorizontal();
    void AddItemMiddle();
    void AddItemDown();

private:
    Ui::MainWindow *ui;
    ItemWidget* CreateNewItem(QString text = "");
    void AddItem(CreationDirection, QObject*);
    int counter = 0, startCol = 0, startRow = 5;
    ItemWidget* mostRecentlyAdded = nullptr;
    int shiftDownIfBelowThis, modifiedRow;

    QString getStyleSheetString(QString color){
        // This fuckery is so that the children don't inherit the parent's style sheet
        QString settingStyle =
                QString("ItemWidget {background-color:") +
                color +
                QString(";border-radius: 10px;}");
        return settingStyle;
    }

    // There's definitely a better way to pick colors.
    std::vector<QColor> colors = {
        QColor("darkGray"),
        QColor("green"),
        QColor("blue"),
        QColor("cyan"),
        QColor("magenta"),
        QColor("red"),
        QColor("darkRed"),
        QColor("darkCyan"),
        QColor("darkMagenta"),
        QColor("darkGreen"),
        QColor("yellow"),
    };
    int colorIndex = 0;

};

#endif // MAINWINDOW_H
