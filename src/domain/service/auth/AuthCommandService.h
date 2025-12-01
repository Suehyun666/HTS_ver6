#pragma once
#include "IAuthCommandService.h"

class AuthCommandService : public IAuthCommandService {
public:
    AuthCommandService() = default;

    void loginAsync(
        const QString& userId,
        const QString& password,
        QObject* context,
        std::function<void(Result<Session>)> callback
    ) override;

    void logoutAsync(
        QObject* context,
        std::function<void(Result<void>)> callback
    ) override;
};
