#pragma once
#include <QString>
#include <QMutex>
#include <QQueue>
#include <QAbstractGrpcChannel>
#include <memory>
#include "../../core/model/Result.h"

class QAbstractGrpcChannel;

struct RetryPolicy {
    int maxRetries = 3;
    int initialBackoffMs = 100;
    int maxBackoffMs = 5000;
    double backoffMultiplier = 2.0;
};

struct TimeoutConfig {
    int connectTimeoutMs = 5000;
    int requestTimeoutMs = 10000;
};

class GrpcChannelPool {
public:
    static GrpcChannelPool& instance();

    Result<void> initialize(const QString& host, int port);
    void shutdown();

    std::shared_ptr<QAbstractGrpcChannel> acquireChannel();
    void releaseChannel(std::shared_ptr<QAbstractGrpcChannel> channel);

    void setRetryPolicy(const RetryPolicy& policy);
    void setTimeoutConfig(const TimeoutConfig& config);

    const RetryPolicy& retryPolicy() const { return retryPolicy_; }
    const TimeoutConfig& timeoutConfig() const { return timeoutConfig_; }

private:
    GrpcChannelPool() = default;
    ~GrpcChannelPool();

    std::shared_ptr<QAbstractGrpcChannel> createChannel();

    QString host_;
    int port_ = 50051;

    QQueue<std::shared_ptr<QAbstractGrpcChannel>> pool_;
    int activeChannels_ = 0;

    RetryPolicy retryPolicy_;
    TimeoutConfig timeoutConfig_;

    QMutex mutex_;
    bool initialized_ = false;

    static constexpr int MAX_CHANNELS = 10;
};
