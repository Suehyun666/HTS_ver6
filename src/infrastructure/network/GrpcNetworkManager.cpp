#include "GrpcNetworkManager.h"
#include <QDebug>

GrpcNetworkManager& GrpcNetworkManager::instance() {
    static GrpcNetworkManager inst;
    return inst;
}

GrpcNetworkManager::GrpcNetworkManager(QObject* parent)
    : QObject(parent)
{
}

GrpcNetworkManager::~GrpcNetworkManager() {
    shutdown();
}

Result<void> GrpcNetworkManager::initialize() {
    authClient_ = new auth::AuthService::Client(this);
    orderClient_ = new order::OrderService::Client(this);

    qDebug() << "[GrpcNetworkManager] Initialized with Auth and Order clients";
    return Result<void>::success();
}

void GrpcNetworkManager::shutdown() {
    if (authClient_) {
        delete authClient_;
        authClient_ = nullptr;
    }

    if (orderClient_) {
        delete orderClient_;
        orderClient_ = nullptr;
    }

    qDebug() << "[GrpcNetworkManager] Shutdown complete";
}
