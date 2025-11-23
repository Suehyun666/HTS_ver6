#include "LoginViewModel.h"
#include "../../infrastructure/session/SessionManager.h"
#include "../../infrastructure/network/NetworkExecutor.h"
#include <QMetaObject>

LoginViewModel::LoginViewModel(std::unique_ptr<IAuthCommandService> authService, QObject* parent)
    : QObject(parent)
    , authService_(std::move(authService))
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

    // 비동기 실행 (UI 블로킹 방지)
    NetworkExecutor::instance().execute([this, userId, password]() {
        auto result = authService_->login(userId, password);

        // UI 쓰레드로 결과 전달
        QMetaObject::invokeMethod(this, [this, result]() {
            setBusy(false);

            if (result.isError()) {
                setErrorMessage(result.error().message);
                emit loginFailed(result.error().message);
                return;
            }

            // 세션 저장
            SessionManager::instance().setSession(result.value());
            emit loginSucceeded();
        }, Qt::QueuedConnection);
    });
}
