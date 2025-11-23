#ifndef HTS_VER6_MARKETMENU_H
#define HTS_VER6_MARKETMENU_H

#include <QObject>
#include <QMenu>

class WindowManager;

class MarketMenu : public QObject {
    Q_OBJECT
public:
    explicit MarketMenu(WindowManager* windowManager, QObject* parent = nullptr);

    QMenu* createMenu(QWidget* parent = nullptr);

private:
    WindowManager* windowManager;
};

#endif
