#pragma once
#include "../../model/Session.h"
#include "core/model/Result.h"
#include <QString>
#include <QObject>
#include <functional>

class IAuthCommandService {
public:
    virtual ~IAuthCommandService() = default;

    virtual void loginAsync(
        const QString& userId,
        const QString& password,
        QObject* context,
        std::function<void(Result<Session>)> callback
    ) = 0;

    virtual void logoutAsync(
        QObject* context,
        std::function<void(Result<void>)> callback
    ) = 0;
};
