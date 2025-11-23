#include "OrderMenu.h"
#include "../window/WindowManager.h"
#include "../../ui/order/OrderWidget.h"
#include <QAction>

OrderMenu::OrderMenu(WindowManager* windowManager, QObject* parent)
    : QObject(parent), windowManager(windowManager) {
}

QMenu* OrderMenu::createMenu(QWidget* parent) {
    QMenu* menu = new QMenu(tr("Order"), parent);

    QAction* orderAction = menu->addAction(tr("Order"));
    connect(orderAction, &QAction::triggered, this, [this]() {
        windowManager->openWindow<OrderWidget>();
    });

    return menu;
}
