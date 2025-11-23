#include "AuthCommandService.h"
#include <QThread>

Result<Session> AuthCommandService::login(const QString& userId, const QString& password) {
    // TODO: 실제 gRPC 호출
    // 지금은 Mock
    if (userId.isEmpty() || password.isEmpty()) {
        return Result<Session>::failure(
            ErrorCode::InvalidInput,
            "User ID and password cannot be empty"
        );
    }

    // 시뮬레이션: 네트워크 딜레이
    QThread::msleep(100);

    // Mock 세션 생성
    Session session;
    session.sessionId = "mock_session_" + userId;
    session.userId = userId;
    session.userName = "User " + userId;
    session.loginTime = QDateTime::currentDateTime();
    session.expireTime = QDateTime::currentDateTime().addSecs(3600);

    return Result<Session>::success(std::move(session));
}

Result<void> AuthCommandService::logout() {
    // TODO: gRPC 로그아웃 호출
    return Result<void>::success();
}
