#include "MenuBarManager.h"
#include "WindowManager.h"
#include "WorkspaceManager.h"
#include "ThemeManager.h"
#include "../model/Constants.h"
#include "../notification/NotificationManager.h"
#include "../../ui/order/OrderWidget.h"
#include "../../ui/chart/ChartWidget.h"
#include "../../ui/account/AccountView.h"
#include "../../ui/tools/note/NoteWidget.h"
#include "../../ui/tools/calculator/CalculatorWidget.h"
#include "../../ui/tools/calendar/CalendarWidget.h"
#include "../../ui/tools/clock/ClockWidget.h"
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <QMessageBox>
#include <QKeySequence>

MenuBarManager::MenuBarManager(QMenuBar* menuBar, WindowManager* windowManager, WorkspaceManager* workspaceManager, QObject* parent)
    : QObject(parent), menuBar(menuBar), windowManager(windowManager), workspaceManager(workspaceManager) {
}

void MenuBarManager::setupMenus() {
    createAccountMenu();
    createMarketMenu();
    createOrderMenu();
    createToolMenu();
    createSettingsMenu();
    createTestMenu();
}

void MenuBarManager::createAccountMenu() {
    QMenu* menu = menuBar->addMenu(tr("Account"));

    QAction* accountAction = menu->addAction(tr("Account View"));
    accountAction->setShortcut(QKeySequence("Ctrl+A"));
    connect(accountAction, &QAction::triggered, this, [this]() {
        windowManager->openWindow<AccountView>();
    });
}

void MenuBarManager::createMarketMenu() {
    QMenu* menu = menuBar->addMenu(tr("Market"));

    QAction* chartAction = menu->addAction(tr("Chart"));
    chartAction->setShortcut(QKeySequence("Ctrl+H"));
    connect(chartAction, &QAction::triggered, this, [this]() {
        windowManager->openWindow<ChartWidget>();
    });
}

void MenuBarManager::createOrderMenu() {
    QMenu* menu = menuBar->addMenu(tr("Order"));

    QAction* orderAction = menu->addAction(tr("Order"));
    orderAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(orderAction, &QAction::triggered, this, [this]() {
        windowManager->openWindow<OrderWidget>();
    });
}

void MenuBarManager::createToolMenu() {
    QMenu* menu = menuBar->addMenu(tr("Tools"));

    QAction* calcAction = menu->addAction(tr("Calculator"));
    calcAction->setShortcut(QKeySequence("Ctrl+Shift+C"));
    connect(calcAction, &QAction::triggered, this, [this]() {
        windowManager->openWindow<CalculatorWidget>();
    });

    QAction* calendarAction = menu->addAction(tr("Calendar"));
    calendarAction->setShortcut(QKeySequence("Ctrl+Shift+L"));
    connect(calendarAction, &QAction::triggered, this, [this]() {
        windowManager->openWindow<CalendarWidget>();
    });

    QAction* clockAction = menu->addAction(tr("Clock"));
    clockAction->setShortcut(QKeySequence("Ctrl+Shift+K"));
    connect(clockAction, &QAction::triggered, this, [this]() {
        windowManager->openWindow<ClockWidget>();
    });

    QAction* noteAction = menu->addAction(tr("Note"));
    noteAction->setShortcut(QKeySequence("Ctrl+N"));
    connect(noteAction, &QAction::triggered, this, [this]() {
        windowManager->openWindow<NoteWidget>();
    });

    menu->addSeparator();

    QAction* closeAllAction = menu->addAction(tr("Close All Windows"));
    connect(closeAllAction, &QAction::triggered, this, [this]() {
        windowManager->closeAll();
    });
}

void MenuBarManager::createSettingsMenu() {
    QMenu* menu = menuBar->addMenu(tr("Settings"));

    QMenu* themeMenu = menu->addMenu(tr("Theme"));
    QAction* lightTheme = themeMenu->addAction(tr("Light"));
    QAction* darkTheme = themeMenu->addAction(tr("Dark"));
    QAction* systemTheme = themeMenu->addAction(tr("System"));

    connect(lightTheme, &QAction::triggered, this, []() {
        ThemeManager::instance().loadTheme(ThemeManager::Theme::Light);
    });

    connect(darkTheme, &QAction::triggered, this, []() {
        ThemeManager::instance().loadTheme(ThemeManager::Theme::Dark);
    });

    connect(systemTheme, &QAction::triggered, this, []() {
        ThemeManager::instance().loadSystemTheme();
    });

    menu->addSeparator();

    QMenu* langMenu = menu->addMenu(tr("Language"));
    QAction* koreanLang = langMenu->addAction(tr("Korean"));
    QAction* englishLang = langMenu->addAction(tr("English"));

    connect(koreanLang, &QAction::triggered, this, [this]() {
        QSettings settings(AppConstants::ORGANIZATION_NAME, AppConstants::APP_NAME);
        settings.setValue(AppConstants::SETTINGS_LANGUAGE, "ko_KR");
        QMessageBox::information(nullptr, tr("Language"),
            tr("Language changed. Restart application to apply."));
    });

    connect(englishLang, &QAction::triggered, this, [this]() {
        QSettings settings(AppConstants::ORGANIZATION_NAME, AppConstants::APP_NAME);
        settings.setValue(AppConstants::SETTINGS_LANGUAGE, "en_US");
        QMessageBox::information(nullptr, tr("Language"),
            tr("Language changed. Restart application to apply."));
    });
}

void MenuBarManager::createTestMenu() {
    QMenu* menu = menuBar->addMenu(tr("Test"));

    QAction* testTradeNotification = menu->addAction(tr("Test Trade Notification"));
    connect(testTradeNotification, &QAction::triggered, this, []() {
        NotificationManager::instance().showTradeNotification(
            QObject::tr("Trade Executed"),
            QObject::tr("Buy 100 shares of AAPL at $150.00")
        );
    });

    QAction* testInfoNotification = menu->addAction(tr("Test Info Notification"));
    connect(testInfoNotification, &QAction::triggered, this, []() {
        NotificationManager::instance().showInfoNotification(
            QObject::tr("Market Update"),
            QObject::tr("KOSPI index reached 2,500 points")
        );
    });

    QAction* testWarningNotification = menu->addAction(tr("Test Warning Notification"));
    connect(testWarningNotification, &QAction::triggered, this, []() {
        NotificationManager::instance().showWarningNotification(
            QObject::tr("Price Alert"),
            QObject::tr("Samsung Electronics dropped below 70,000 KRW")
        );
    });
}
