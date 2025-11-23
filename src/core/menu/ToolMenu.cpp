#include "ToolMenu.h"
#include "../window/WindowManager.h"
#include "../../ui/tools/calculator/CalculatorWidget.h"
#include "../../ui/tools/calendar/CalendarWidget.h"
#include "../../ui/tools/clock/ClockWidget.h"
#include "../../ui/tools/note/NoteWidget.h"
#include <QAction>

ToolMenu::ToolMenu(WindowManager* windowManager, QObject* parent)
    : QObject(parent), windowManager(windowManager) {
}

QMenu* ToolMenu::createMenu(QWidget* parent) {
    QMenu* menu = new QMenu(tr("Tools"), parent);

    QAction* calcAction = menu->addAction(tr("Calculator"));
    connect(calcAction, &QAction::triggered, this, [this]() {
        windowManager->openWindow<CalculatorWidget>();
    });

    QAction* calendarAction = menu->addAction(tr("Calendar"));
    connect(calendarAction, &QAction::triggered, this, [this]() {
        windowManager->openWindow<CalendarWidget>();
    });

    QAction* clockAction = menu->addAction(tr("Clock"));
    connect(clockAction, &QAction::triggered, this, [this]() {
        windowManager->openWindow<ClockWidget>();
    });

    QAction* noteAction = menu->addAction(tr("Note"));
    connect(noteAction, &QAction::triggered, this, [this]() {
        windowManager->openWindow<NoteWidget>();
    });

    return menu;
}
