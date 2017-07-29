#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QtCore>
#include <vector>
#include <QWheelEvent>
#include <unordered_map>
#include <QColor>

class DrawWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DrawWidget(QWidget *parent = nullptr);
    void updateEndpoints(QWidget* startIn, QWidget* endIn, QColor* newRowColor = nullptr);
    virtual void paintEvent(QPaintEvent *);
    virtual void wheelEvent(QWheelEvent *event);
    std::unordered_map<QWidget*, QColor*> assignedColors;
signals:

private:
    struct ConnectedPair {
        QWidget* from;
        QWidget* to;
    };

    QString getStyleSheetString(QString color){
        // This fuckery is so that the children don't inherit the parent's style sheet
        QString settingStyle =
                QString("ItemWidget {background-color:") +
                color +
                QString(";border-radius: 10px;}");
        return settingStyle;
    }

    std::vector<ConnectedPair> items;
    int scrollDelta = 0;
};

#endif // DRAWWIDGET_H
