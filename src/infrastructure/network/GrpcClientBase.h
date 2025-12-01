#pragma once
#include <QObject>
#include <QAbstractGrpcChannel>
#include <QGrpcStatus>
#include <QThread>
#include <functional>
#include "GrpcChannelPool.h"
#include "../../core/model/Result.h"

template<typename TRequest, typename TResponse>
class GrpcClientBase : public QObject {
public:
    using RequestFunc = std::function<void(QAbstractGrpcChannel*, const TRequest&, QObject*, std::function<void(const TResponse&)>, std::function<void(QGrpcStatus)>)>;

    explicit GrpcClientBase(QObject* parent = nullptr)
        : QObject(parent) {}

    Result<TResponse> executeWithRetry(const TRequest& request, RequestFunc requestFunc) {
        auto& pool = GrpcChannelPool::instance();
        const auto& policy = pool.retryPolicy();

        int attempt = 0;
        int backoffMs = policy.initialBackoffMs;

        while (attempt <= policy.maxRetries) {
            QAbstractGrpcChannel* channel = pool.acquireChannel();
            if (!channel) {
                return Result<TResponse>::failure(
                    ErrorCode::GrpcConnectionFailed,
                    "Failed to acquire channel"
                );
            }

            TResponse response;
            QGrpcStatus status;
            bool completed = false;

            requestFunc(
                channel,
                request,
                this,
                [&response, &completed](const TResponse& resp) {
                    response = resp;
                    completed = true;
                },
                [&status, &completed](QGrpcStatus stat) {
                    status = stat;
                    completed = true;
                }
            );

            while (!completed) {
                QCoreApplication::processEvents();
            }

            pool.releaseChannel(channel);

            if (status.code() == QGrpcStatus::StatusCode::Ok) {
                return Result<TResponse>::success(std::move(response));
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

        return Result<TResponse>::failure(
            ErrorCode::GrpcConnectionFailed,
            QString("Request failed after %1 retries").arg(policy.maxRetries)
        );
    }
};
