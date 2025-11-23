#include "ClockWidget.h"
#include "DigitalClock.h"
#include <QVBoxLayout>

ClockWidget::ClockWidget(QWidget* parent) : DomainWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    clock = new DigitalClock(this);
    layout->addWidget(clock);
    setLayout(layout);
}
