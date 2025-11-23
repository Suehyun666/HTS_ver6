#include "calendar.h"
#include <QCalendarWidget>
#include <QVBoxLayout>

Window::Window(QWidget *parent)
    : QWidget(parent)
{
    calendar = new QCalendarWidget(this);
    calendar->setGridVisible(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(calendar);
    setLayout(layout);
}
