#ifndef HTS_VER6_DOMAINWIDGET_H
#define HTS_VER6_DOMAINWIDGET_H
#include <QWidget>
#include <QJsonObject>
#include <QSize>

class DomainWidget : public QWidget {
    Q_OBJECT
public:
    explicit DomainWidget(QWidget* parent = nullptr);

    virtual QString windowId() const = 0;
    virtual QString windowTitle() const = 0;
    virtual bool isSingleton() const { return false; }

    virtual void saveState(QJsonObject& state);
    virtual void loadState(const QJsonObject& state);

    virtual void onActivated();
    virtual void onDeactivated();

    virtual bool canClose() const;

protected:
    // Size constraints (override in subclass if needed)
    virtual QSize preferredSize() const { return QSize(800, 600); }
    virtual QSize minimumSizeHint() const override { return QSize(400, 300); }
    virtual QSize maximumSizeHint() const { return QSize(1600, 1200); }

    // Theme support
    virtual void applyTheme();
    void setupSizeConstraints();

    // Translation support
    virtual void retranslateUi();

private slots:
    void onThemeChanged();
    void onLanguageChanged(const QString& languageCode);

signals:
    void requestClose();
};

#endif