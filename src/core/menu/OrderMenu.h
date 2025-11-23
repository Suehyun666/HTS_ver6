#ifndef HTS_VER6_ORDERMENU_H
#define HTS_VER6_ORDERMENU_H

#include <QObject>
#include <QMenu>

class WindowManager;

class OrderMenu : public QObject {
    Q_OBJECT
public:
    explicit OrderMenu(WindowManager* windowManager, QObject* parent = nullptr);

    QMenu* createMenu(QWidget* parent = nullptr);

private:
    WindowManager* windowManager;
};

#endif
