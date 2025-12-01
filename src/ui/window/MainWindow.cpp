#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "../../core/window/WindowManager.h"
#include "../../core/window/MenuBarManager.h"
#include "../../core/window/ThemeManager.h"
#include "../../core/notification/NotificationManager.h"
#include "../../core/model/Constants.h"
#include <QIcon>
#include <QMdiArea>
#include <QPalette>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(AppConstants::ICON_APP));
    setMinimumSize(AppConstants::MIN_WINDOW_WIDTH, AppConstants::MIN_WINDOW_HEIGHT);

    // Apply theme to window (including title bar hint)
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, [this]() {
        // Update window palette to match theme
        setPalette(qApp->palette());
    });

    mdiArea = new QMdiArea(this);
    setCentralWidget(mdiArea);

    windowManager = new WindowManager(mdiArea, this);
    menuBarManager = new MenuBarManager(ui->menubar, windowManager, nullptr, this);

    menuBarManager->setupMenus();

    NotificationManager::instance().initialize();

    // 화면 중앙에 거의 전체화면으로 표시
    centerOnScreen();
    showMaximized();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::centerOnScreen() {
    // showMaximized() 전에 초기 위치만 화면 중앙으로
    move(100, 100);
}
