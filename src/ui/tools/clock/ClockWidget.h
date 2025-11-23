#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

#include "../../widget/DomainWidget.h"

class DigitalClock;

class ClockWidget : public DomainWidget {
    Q_OBJECT
public:
    explicit ClockWidget(QWidget* parent = nullptr);

    QString windowId() const override { return "clock"; }
    QString windowTitle() const override { return tr("Clock"); }
    bool isSingleton() const override { return true; }

protected:
    QSize preferredSize() const override { return QSize(300, 150); }
    QSize minimumSizeHint() const override { return QSize(200, 100); }
    QSize maximumSizeHint() const override { return QSize(500, 300); }

private:
    DigitalClock* clock;
};

#endif
