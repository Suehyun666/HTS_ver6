#ifndef HTS_VER6_WORKSPACEMANAGER_H
#define HTS_VER6_WORKSPACEMANAGER_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>

class WindowManager;
class QMdiArea;

class WorkspaceManager : public QObject {
    Q_OBJECT
public:
    explicit WorkspaceManager(WindowManager* windowManager, QMdiArea* mdiArea, QObject* parent = nullptr);

    bool saveWorkspace(const QString& name);

    bool loadWorkspace(const QString& name);

    QStringList availableWorkspaces() const;

    bool deleteWorkspace(const QString& name);

private:
    WindowManager* windowManager;
    QMdiArea* mdiArea;
    QString workspaceDir;

    QString getWorkspaceFilePath(const QString& name) const;

    QJsonObject captureCurrentLayout() const;

    void restoreLayout(const QJsonObject& layout);
};

#endif
