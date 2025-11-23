#pragma once
#include "../../domain/model/Session.h"
#include <QMutex>
#include <optional>

class SessionManager {
public:
    static SessionManager& instance();

    void setSession(const Session& session);
    std::optional<Session> currentSession() const;
    bool isLoggedIn() const;
    void clear();

private:
    SessionManager() = default;

    mutable QMutex mutex_;
    std::optional<Session> session_;
};
