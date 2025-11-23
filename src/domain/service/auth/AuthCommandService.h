#pragma once
#include "IAuthCommandService.h"

// gRPC 클라이언트는 나중에 구현
// 지금은 Mock으로
class AuthCommandService : public IAuthCommandService {
public:
    AuthCommandService() = default;

    Result<Session> login(const QString& userId, const QString& password) override;
    Result<void> logout() override;

private:
    // TODO: GrpcChannelPool에서 받아온 채널 저장
};
