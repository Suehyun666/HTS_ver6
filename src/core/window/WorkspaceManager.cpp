#include "WorkspaceManager.h"
#include "WindowManager.h"
#include "../../ui/widget/DomainWidget.h"
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

WorkspaceManager::WorkspaceManager(WindowManager* windowManager, QMdiArea* mdiArea, QObject* parent)
    : QObject(parent), windowManager(windowManager), mdiArea(mdiArea) {
    workspaceDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/workspaces";
    QDir().mkpath(workspaceDir);
}

bool WorkspaceManager::saveWorkspace(const QString& name) {
    QJsonObject layout = captureCurrentLayout();

    QJsonObject root;
    root["version"] = 1;
    root["name"] = name;
    root["layout"] = layout;

    QString filePath = getWorkspaceFilePath(name);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonDocument doc(root);
    file.write(doc.toJson());
    file.close();

    return true;
}

bool WorkspaceManager::loadWorkspace(const QString& name) {
    QString filePath = getWorkspaceFilePath(name);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        return false;
    }

    QJsonObject root = doc.object();
    QJsonObject layout = root["layout"].toObject();

    windowManager->closeAll();

    restoreLayout(layout);

    return true;
}

QStringList WorkspaceManager::availableWorkspaces() const {
    QDir dir(workspaceDir);
    QStringList filters;
    filters << "*.json";
    QStringList files = dir.entryList(filters, QDir::Files);

    QStringList names;
    for (const QString& file : files) {
        names.append(file.left(file.length() - 5));
    }
    return names;
}

bool WorkspaceManager::deleteWorkspace(const QString& name) {
    QString filePath = getWorkspaceFilePath(name);
    return QFile::remove(filePath);
}

QString WorkspaceManager::getWorkspaceFilePath(const QString& name) const {
    return workspaceDir + "/" + name + ".json";
}

QJsonObject WorkspaceManager::captureCurrentLayout() const {
    QJsonObject layout;
    QJsonArray windows;

    QList<QMdiSubWindow*> subWindows = mdiArea->subWindowList();
    for (QMdiSubWindow* sub : subWindows) {
        DomainWidget* widget = qobject_cast<DomainWidget*>(sub->widget());
        if (!widget) {
            continue;
        }

        QJsonObject winObj;
        winObj["type"] = widget->windowId();
        winObj["title"] = widget->windowTitle();

        QRect geom = sub->geometry();
        QJsonObject geomObj;
        geomObj["x"] = geom.x();
        geomObj["y"] = geom.y();
        geomObj["width"] = geom.width();
        geomObj["height"] = geom.height();
        winObj["geometry"] = geomObj;

        winObj["maximized"] = sub->isMaximized();
        winObj["minimized"] = sub->isMinimized();

        QJsonObject state;
        widget->saveState(state);
        winObj["state"] = state;

        windows.append(winObj);
    }

    layout["windows"] = windows;
    return layout;
}

void WorkspaceManager::restoreLayout(const QJsonObject& layout) {
}
