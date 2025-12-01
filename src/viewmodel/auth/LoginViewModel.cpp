#include "LoginViewModel.h"
#include "../../infrastructure/session/SessionManager.h"
#include "../../infrastructure/network/NetworkExecutor.h"
#include <QMetaObject>

LoginViewModel::LoginViewModel(std::unique_ptr<IAuthCommandService> authService, QObject* parent)
    : QObject(parent)
    , authService(std::move(authService))
{
}

void LoginViewModel::setUserId(const QString& id) {
    if (userId_ != id) {
        userId_ = id;
        emit userIdChanged();
    }
}

void LoginViewModel::setPassword(const QString& pw) {
    if (password_ != pw) {
        password_ = pw;
        emit passwordChanged();
    }
}

void LoginViewModel::setBusy(bool busy) {
    if (busy_ != busy) {
        busy_ = busy;
        emit busyChanged();
    }
}

void LoginViewModel::setErrorMessage(const QString& msg) {
    if (errorMessage_ != msg) {
        errorMessage_ = msg;
        emit errorMessageChanged();
    }
}

void LoginViewModel::loginCommand() {
    if (busy_) {
        return;
    }

    setBusy(true);
    setErrorMessage("");

    auto userId = userId_;
    auto password = password_;

    authService->loginAsync(userId, password, this, [this](Result<Session> result) {
        setBusy(false);

        if (result.isError()) {
            setErrorMessage(result.error().message);
            emit loginFailed(result.error().message);
            return;
        }

        emit loginSucceeded();
    });
}
