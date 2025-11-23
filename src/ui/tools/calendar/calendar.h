#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

class QCalendarWidget;

class Window : public QWidget
{
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);

private:
    QCalendarWidget *calendar;
};

#endif
