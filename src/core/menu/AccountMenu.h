#ifndef HTS_VER6_ACCOUNTMENU_H
#define HTS_VER6_ACCOUNTMENU_H

#include <QObject>
#include <QMenu>

class WindowManager;

class AccountMenu : public QObject {
    Q_OBJECT
public:
    explicit AccountMenu(WindowManager* windowManager, QObject* parent = nullptr);

    QMenu* createMenu(QWidget* parent = nullptr);

private:
    WindowManager* windowManager;
};

#endif
