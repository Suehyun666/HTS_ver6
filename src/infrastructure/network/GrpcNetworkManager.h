#pragma once
#include <QObject>
#include <QEventLoop>
#include <QThread>
#include <memory>
#include <functional>
#include "GrpcChannelPool.h"
#include "../../core/model/Result.h"
#include <auth_api_client.grpc.qpb.h>
#include <order_api_client.grpc.qpb.h>

class GrpcNetworkManager : public QObject {
    Q_OBJECT

public:
    static GrpcNetworkManager& instance();

    Result<void> initialize();
    void shutdown();

    template<typename TRequest, typename TReply>
    using GrpcCall = std::function<std::shared_ptr<QGrpcCallReply>(
        QAbstractGrpcClient*, const TRequest&, const QGrpcCallOptions&)>;

    template<typename TRequest, typename TReply>
    Result<TReply> executeCall(
        QAbstractGrpcClient* client,
        const TRequest& request,
        GrpcCall<TRequest, TReply> callFunc
    );

    template<typename TRequest, typename TReply>
    void executeCallAsync(
        QAbstractGrpcClient* client,
        const TRequest& request,
        GrpcCall<TRequest, TReply> callFunc,
        QObject* context,
        std::function<void(Result<TReply>)> callback
    );

    auth::AuthService::Client* authClient() { return authClient_; }
    order::OrderService::Client* orderClient() { return orderClient_; }

private:
    explicit GrpcNetworkManager(QObject* parent = nullptr);
    ~GrpcNetworkManager();

    auth::AuthService::Client* authClient_ = nullptr;
    order::OrderService::Client* orderClient_ = nullptr;
};

template<typename TRequest, typename TReply>
Result<TReply> GrpcNetworkManager::executeCall(
    QAbstractGrpcClient* client,
    const TRequest& request,
    GrpcCall<TRequest, TReply> callFunc
) {
    auto& pool = GrpcChannelPool::instance();
    const auto& policy = pool.retryPolicy();

    int attempt = 0;
    int backoffMs = policy.initialBackoffMs;

    while (attempt <= policy.maxRetries) {
        auto channel = pool.acquireChannel();
        if (!channel) {
            return Result<TReply>::failure(
                ErrorCode::GrpcConnectionFailed,
                "Failed to acquire channel"
            );
        }

        client->attachChannel(channel);

        QEventLoop loop;
        TReply reply;
        bool hasError = false;
        QString errorMessage;

        auto grpcReply = callFunc(client, request, QGrpcCallOptions{});

        QObject::connect(grpcReply.get(), &QGrpcCallReply::finished, this, [&, grpcReply]() {
            reply = grpcReply->template read<TReply>();
            loop.quit();
        });

        QObject::connect(grpcReply.get(), &QGrpcCallReply::errorOccurred, this, [&](const QGrpcStatus& status) {
            hasError = true;
            errorMessage = status.message();
            loop.quit();
        });

        loop.exec();

        pool.releaseChannel(channel);

        if (!hasError) {
            return Result<TReply>::success(std::move(reply));
        }

        attempt++;
        if (attempt <= policy.maxRetries) {
            QThread::msleep(backoffMs);
            backoffMs = qMin(
                static_cast<int>(backoffMs * policy.backoffMultiplier),
                policy.maxBackoffMs
            );
        }
    }

    return Result<TReply>::failure(
        ErrorCode::GrpcConnectionFailed,
        QString("Request failed after %1 retries").arg(policy.maxRetries)
    );
}

template<typename TRequest, typename TReply>
void GrpcNetworkManager::executeCallAsync(
    QAbstractGrpcClient* client,
    const TRequest& request,
    GrpcCall<TRequest, TReply> callFunc,
    QObject* context,
    std::function<void(Result<TReply>)> callback
) {
    auto channel = GrpcChannelPool::instance().acquireChannel();
    if (!channel) {
        callback(Result<TReply>::failure(
            ErrorCode::GrpcConnectionFailed,
            "Failed to acquire channel"
        ));
        return;
    }

    client->attachChannel(channel);

    auto grpcReply = callFunc(client, request, QGrpcCallOptions{});

    QObject::connect(grpcReply.get(), &QGrpcCallReply::finished, context, [this, callback, grpcReply, channel]() {
        auto reply = grpcReply->template read<TReply>();
        GrpcChannelPool::instance().releaseChannel(channel);
        callback(Result<TReply>::success(std::move(reply)));
    });

    QObject::connect(grpcReply.get(), &QGrpcCallReply::errorOccurred, context, [this, callback, channel](const QGrpcStatus& status) {
        GrpcChannelPool::instance().releaseChannel(channel);
        callback(Result<TReply>::failure(
            ErrorCode::GrpcConnectionFailed,
            status.message()
        ));
    });
}
