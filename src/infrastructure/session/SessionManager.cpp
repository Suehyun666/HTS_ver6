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

bool SessionManager::isLoggedIn() const {
    QMutexLocker lock(&mutex_);
    return session_.has_value() && session_->isValid();
}

void SessionManager::clear() {
    QMutexLocker lock(&mutex_);
    session_.reset();
}
