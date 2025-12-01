#ifndef HTS_VER6_WINDOWMANAGER_H
#define HTS_VER6_WINDOWMANAGER_H

#include <QObject>
#include <QMdiArea>
#include <QMdiSubWindow>

class DomainWidget;

class WindowManager : public QObject {
    Q_OBJECT
public:
    explicit WindowManager(QMdiArea* mdiArea, QObject* parent = nullptr);

    template<typename T>
    QMdiSubWindow* openWindow();

    QMdiSubWindow* findWindow(const QString& windowId);

    void closeAll();

private:
    QMdiArea* mdiArea;

    DomainWidget* createWidgetInstance(const QString& type);
    QMdiSubWindow* addWidgetToMdi(QWidget* widget);
};

#endif
