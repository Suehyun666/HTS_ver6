#ifndef HTS_VER6_MENUBARMANAGER_H
#define HTS_VER6_MENUBARMANAGER_H

#include <QObject>
#include <QMenuBar>

class WindowManager;
class WorkspaceManager;

class MenuBarManager : public QObject {
    Q_OBJECT
public:
    explicit MenuBarManager(QMenuBar* menuBar, WindowManager* windowManager, WorkspaceManager* workspaceManager = nullptr, QObject* parent = nullptr);

    void setupMenus();

private:
    QMenuBar* menuBar;
    WindowManager* windowManager;
    WorkspaceManager* workspaceManager;

    void createAccountMenu();
    void createMarketMenu();
    void createOrderMenu();
    void createToolMenu();
    void createSettingsMenu();
    void createTestMenu();
};

#endif
