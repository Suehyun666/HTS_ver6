#ifndef HTS_VER6_TOOLMENU_H
#define HTS_VER6_TOOLMENU_H

#include <QObject>
#include <QMenu>

class WindowManager;

class ToolMenu : public QObject {
    Q_OBJECT
public:
    explicit ToolMenu(WindowManager* windowManager, QObject* parent = nullptr);

    QMenu* createMenu(QWidget* parent = nullptr);

private:
    WindowManager* windowManager;
};

#endif
