#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>

class NotificationManager : public QObject {
    Q_OBJECT
public:
    static NotificationManager& instance();

    void initialize();

    void showTradeNotification(const QString& title, const QString& message);
    void showInfoNotification(const QString& title, const QString& message);
    void showWarningNotification(const QString& title, const QString& message);

private:
    NotificationManager();
    QSystemTrayIcon* trayIcon;

    void playSound(const QString& soundPath);
};

#endif
