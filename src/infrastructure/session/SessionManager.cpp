#include "SessionManager.h"
#include <QMutexLocker>

SessionManager& SessionManager::instance() {
    static SessionManager inst;
    return inst;
}

void SessionManager::setSession(const Session& session) {
    QMutexLocker lock(&mutex_);
    session_ = session;
}

std::optional<Session> SessionManager::currentSession() const {
    QMutexLocker lock(&mutex_);
    return session_;
}

QString SessionManager::currentSessionId() const {
    QMutexLocker lock(&mutex_);
    if (session_.has_value()) {
        return session_->sessionId;
    }
    return QString();
}

qint64 SessionManager::currentAccountId() const {
    QMutexLocker lock(&mutex_);
    if (session_.has_value()) {
        return session_->accountId;
    }
    return 0;
}

bool SessionManager::isLoggedIn() const {
    QMutexLocker lock(&mutex_);
    return session_.has_value() && session_->isValid();
}

void SessionManager::clear() {
    QMutexLocker lock(&mutex_);
    session_.reset();
}
