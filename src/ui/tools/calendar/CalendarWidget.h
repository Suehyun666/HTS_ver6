#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include "../../widget/DomainWidget.h"

class Window;

class CalendarWidget : public DomainWidget {
    Q_OBJECT
public:
    explicit CalendarWidget(QWidget* parent = nullptr);

    QString windowId() const override { return "calendar"; }
    QString windowTitle() const override { return tr("Calendar"); }
    bool isSingleton() const override { return true; }

protected:
    QSize preferredSize() const override;
    QSize minimumSizeHint() const override;
    QSize maximumSizeHint() const override;

private:
    Window* calendarWidget;
};

#endif
