#include "NetworkExecutor.h"

NetworkExecutor& NetworkExecutor::instance() {
    static NetworkExecutor inst;
    return inst;
}

Result<void> NetworkExecutor::initialize(int maxThreads) {
    if (pool_) {
        return Result<void>::failure(
            ErrorCode::NetworkInitFailed,
            "NetworkExecutor already initialized"
        );
    }

    pool_ = new QThreadPool();
    pool_->setMaxThreadCount(maxThreads);

    return Result<void>::success();
}

void NetworkExecutor::shutdown() {
    if (pool_) {
        pool_->waitForDone();
        delete pool_;
        pool_ = nullptr;
    }
}

NetworkExecutor::~NetworkExecutor() {
    shutdown();
}
