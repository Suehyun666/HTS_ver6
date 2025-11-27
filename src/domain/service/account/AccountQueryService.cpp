#include "AccountQueryService.h"
#include <QThread>
#include "infrastructure/session/SessionManager.h"

Result<AccountSnapshot> AccountQueryService::getAccountSnapshot() {
    if (!SessionManager::instance().isLoggedIn()) {
        return Result<AccountSnapshot>::failure(
            ErrorCode::SessionExpired,
            "Session is not valid"
        );
    }

    // TODO: 실제 gRPC 조회
    // Mock 데이터
    QThread::msleep(50);

    AccountSnapshot snapshot;
    snapshot.accountId = "ACC001";
    snapshot.totalAssets = 10000000.0;
    snapshot.cashBalance = 3000000.0;
    snapshot.stockValuation = 7000000.0;
    snapshot.purchasePower = 3000000.0;

    return Result<AccountSnapshot>::success(std::move(snapshot));
}
