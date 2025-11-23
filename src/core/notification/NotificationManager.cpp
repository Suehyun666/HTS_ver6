#include "NotificationManager.h"
#include "../model/Constants.h"
#include <QApplication>
#include <QMenu>
#include <QSoundEffect>
#include <QMessageBox>
#include <QUrl>

NotificationManager& NotificationManager::instance() {
    static NotificationManager instance;
    return instance;
}

NotificationManager::NotificationManager() : trayIcon(nullptr) {
}

void NotificationManager::initialize() {
    if (trayIcon) return;

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }

    trayIcon = new QSystemTrayIcon(QIcon(AppConstants::ICON_TRAY), qApp);

    QMenu* trayMenu = new QMenu();
    QAction* showAction = trayMenu->addAction(tr("Show"));
    QAction* quitAction = trayMenu->addAction(tr("Quit"));

    connect(showAction, &QAction::triggered, []() {
        if (qApp->activeWindow()) {
            qApp->activeWindow()->show();
            qApp->activeWindow()->raise();
            qApp->activeWindow()->activateWindow();
        }
    });

    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

    connect(trayIcon, &QSystemTrayIcon::activated, [](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick) {
            if (qApp->activeWindow()) {
                qApp->activeWindow()->show();
                qApp->activeWindow()->raise();
            }
        }
    });
}

void NotificationManager::showTradeNotification(const QString& title, const QString& message) {
    if (trayIcon && trayIcon->isVisible()) {
        trayIcon->showMessage(title, message, QSystemTrayIcon::Critical, 5000);
        playSound(AppConstants::SOUND_NOTIFICATION);
    } else {
        QMessageBox::information(nullptr, title, message);
    }
}

void NotificationManager::showInfoNotification(const QString& title, const QString& message) {
    if (trayIcon && trayIcon->isVisible()) {
        trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 3000);
        playSound(AppConstants::SOUND_INFO);
    } else {
        QMessageBox::information(nullptr, title, message);
    }
}

void NotificationManager::showWarningNotification(const QString& title, const QString& message) {
    if (trayIcon && trayIcon->isVisible()) {
        trayIcon->showMessage(title, message, QSystemTrayIcon::Warning, 5000);
        playSound(AppConstants::SOUND_ALERT);
    } else {
        QMessageBox::warning(nullptr, title, message);
    }
}

void NotificationManager::playSound(const QString& soundPath) {
    QSoundEffect* sound = new QSoundEffect();

    // Qt resource paths need qrc:// prefix
    QString url = soundPath;
    if (soundPath.startsWith(":/")) {
        url = "qrc" + soundPath;
    }

    sound->setSource(QUrl(url));
    sound->setVolume(0.5f);
    sound->play();

    QObject::connect(sound, &QSoundEffect::playingChanged, sound, [sound]() {
        if (!sound->isPlaying()) {
            sound->deleteLater();
        }
    });
}
