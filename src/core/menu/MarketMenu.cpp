#include "MarketMenu.h"
#include "../window/WindowManager.h"
#include "../../ui/chart/ChartWidget.h"
#include <QAction>

MarketMenu::MarketMenu(WindowManager* windowManager, QObject* parent)
    : QObject(parent), windowManager(windowManager) {
}

QMenu* MarketMenu::createMenu(QWidget* parent) {
    QMenu* menu = new QMenu(tr("Market"), parent);

    QAction* chartAction = menu->addAction(tr("Chart"));
    connect(chartAction, &QAction::triggered, this, [this]() {
        windowManager->openWindow<ChartWidget>();
    });

    return menu;
}
