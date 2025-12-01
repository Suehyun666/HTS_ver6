#include "AuthCommandService.h"
#include "../../../infrastructure/network/GrpcNetworkManager.h"
#include "../../../infrastructure/session/SessionManager.h"
#include <QDebug>
#include <auth_api.qpb.h>

void AuthCommandService::loginAsync(
    const QString& userId,
    const QString& password,
    QObject* context,
    std::function<void(Result<Session>)> callback
) {
    if (userId.isEmpty() || password.isEmpty()) {
        callback(Result<Session>::failure(
            ErrorCode::InvalidInput,
            "User ID and password cannot be empty"
        ));
        return;
    }

    auth::LoginRequest request;
    request.setAccountId(userId.toLongLong());
    request.setPassword(password);
    request.setIpAddr("127.0.0.1");
    request.setUserAgent("HTS_Qt6");

    auto& networkMgr = GrpcNetworkManager::instance();
    networkMgr.executeCallAsync<auth::LoginRequest, auth::LoginReply>(
        networkMgr.authClient(),
        request,
        [](QAbstractGrpcClient* client, const auth::LoginRequest& req, const QGrpcCallOptions& opts) {
            return static_cast<auth::AuthService::Client*>(client)->Login(req, opts);
        },
        context,
        [callback](Result<auth::LoginReply> result) {
            if (result.isError()) {
                callback(Result<Session>::failure(result.error().code, result.error().message));
                return;
            }

            auto& reply = result.value();

            if (reply.code() != auth::AuthResultGadget::AuthResult::SUCCESS) {
                ErrorCode errorCode = ErrorCode::AuthenticationFailed;
                QString msg = "Login failed";

                switch (reply.code()) {
                    case auth::AuthResultGadget::AuthResult::INVALID_CREDENTIALS:
                        msg = "Invalid credentials";
                        break;
                    case auth::AuthResultGadget::AuthResult::ACCOUNT_NOT_FOUND:
                        msg = "Account not found";
                        break;
                    case auth::AuthResultGadget::AuthResult::ACCOUNT_SUSPENDED:
                        msg = "Account suspended";
                        break;
                    case auth::AuthResultGadget::AuthResult::ACCOUNT_LOCKED:
                        msg = "Account locked";
                        break;
                    default:
                        msg = "Authentication failed";
                        break;
                }

                callback(Result<Session>::failure(errorCode, msg));
                return;
            }

            Session session;
            session.sessionId = reply.sessionId();
            session.accountId = reply.accountId();
            session.loginTime = QDateTime::currentDateTime();
            session.expireTime = QDateTime::currentDateTime().addSecs(3600);

            SessionManager::instance().setSession(session);
            callback(Result<Session>::success(std::move(session)));
        }
    );
}

void AuthCommandService::logoutAsync(
    QObject* context,
    std::function<void(Result<void>)> callback
) {
    QString sessionId = SessionManager::instance().currentSessionId();
    if (sessionId.isEmpty()) {
        callback(Result<void>::failure(
            ErrorCode::InvalidInput,
            "No active session"
        ));
        return;
    }

    auth::LogoutRequest request;
    request.setSessionId(sessionId);

    auto& networkMgr = GrpcNetworkManager::instance();
    networkMgr.executeCallAsync<auth::LogoutRequest, auth::LogoutReply>(
        networkMgr.authClient(),
        request,
        [](QAbstractGrpcClient* client, const auth::LogoutRequest& req, const QGrpcCallOptions& opts) {
            return static_cast<auth::AuthService::Client*>(client)->Logout(req, opts);
        },
        context,
        [callback](Result<auth::LogoutReply> result) {
            if (result.isError()) {
                callback(Result<void>::failure(result.error().code, result.error().message));
                return;
            }

            SessionManager::instance().clear();
            callback(Result<void>::success());
        }
    );
}
