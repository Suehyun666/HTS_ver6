#pragma once
#include <QObject>
#include "../../service/auth/IAuthCommandService.h"
#include <memory>

class LoginViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    explicit LoginViewModel(std::unique_ptr<IAuthCommandService> authService, QObject* parent = nullptr);

    QString userId() const { return userId_; }
    void setUserId(const QString& id);

    QString password() const { return password_; }
    void setPassword(const QString& pw);

    bool busy() const { return busy_; }
    QString errorMessage() const { return errorMessage_; }

public slots:
    void loginCommand();

signals:
    void userIdChanged();
    void passwordChanged();
    void busyChanged();
    void errorMessageChanged();

    void loginSucceeded();
    void loginFailed(const QString& reason);

private:
    void setBusy(bool busy);
    void setErrorMessage(const QString& msg);

    std::unique_ptr<IAuthCommandService> authService_;
    QString userId_;
    QString password_;
    bool busy_ = false;
    QString errorMessage_;
};
