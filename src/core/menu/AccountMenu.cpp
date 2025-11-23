#include "AccountMenu.h"
#include "../window/WindowManager.h"
#include "../../ui/account/AccountView.h"
#include <QAction>

AccountMenu::AccountMenu(WindowManager* windowManager, QObject* parent)
    : QObject(parent), windowManager(windowManager) {
}

QMenu* AccountMenu::createMenu(QWidget* parent) {
    QMenu* menu = new QMenu(tr("Account"), parent);

    QAction* accountAction = menu->addAction(tr("Account View"));
    connect(accountAction, &QAction::triggered, this, [this]() {
        windowManager->openWindow<AccountView>();
    });

    return menu;
}
