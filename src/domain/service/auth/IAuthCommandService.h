#pragma once
#include "../../core/model/Result.h"
#include "../../model/Session.h"
#include <QString>

class IAuthCommandService {
public:
    virtual ~IAuthCommandService() = default;

    virtual Result<Session> login(const QString& userId, const QString& password) = 0;
    virtual Result<void> logout() = 0;
};
