#include "CalendarWidget.h"
#include "calendar.h"
#include <QVBoxLayout>

CalendarWidget::CalendarWidget(QWidget* parent) : DomainWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    calendarWidget = new Window(this);
    layout->addWidget(calendarWidget);
    setLayout(layout);
}

QSize CalendarWidget::preferredSize() const {
    return QSize(500, 400);
}

QSize CalendarWidget::minimumSizeHint() const {
    return QSize(400, 350);
}

QSize CalendarWidget::maximumSizeHint() const {
    return QSize(800, 600);
}
