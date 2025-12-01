#include "GrpcChannelPool.h"
#include <QGrpcHttp2Channel>
#include <QGrpcChannelOptions>
#include <QUrl>
#include <QDebug>

GrpcChannelPool& GrpcChannelPool::instance() {
    static GrpcChannelPool inst;
    return inst;
}

GrpcChannelPool::~GrpcChannelPool() {
    shutdown();
}

Result<void> GrpcChannelPool::initialize(const QString& host, int port) {
    QMutexLocker locker(&mutex_);

    if (initialized_) {
        return Result<void>::failure(
            ErrorCode::NetworkInitFailed,
            "GrpcChannelPool already initialized"
        );
    }

    host_ = host;
    port_ = port;
    initialized_ = true;

    qDebug() << "[GrpcChannelPool] Initialized with" << host_ << ":" << port_;
    return Result<void>::success();
}

void GrpcChannelPool::shutdown() {
    QMutexLocker locker(&mutex_);

    if (!initialized_) {
        return;
    }

    pool_.clear();
    activeChannels_ = 0;
    initialized_ = false;

    qDebug() << "[GrpcChannelPool] Shutdown complete";
}

std::shared_ptr<QAbstractGrpcChannel> GrpcChannelPool::acquireChannel() {
    QMutexLocker locker(&mutex_);

    if (!initialized_) {
        qWarning() << "[GrpcChannelPool] Not initialized";
        return nullptr;
    }

    if (!pool_.isEmpty()) {
        auto channel = pool_.dequeue();
        activeChannels_++;
        return channel;
    }

    if (activeChannels_ >= MAX_CHANNELS) {
        qWarning() << "[GrpcChannelPool] Max channels reached";
        return nullptr;
    }

    auto channel = createChannel();
    if (channel) {
        activeChannels_++;
    }
    return channel;
}

void GrpcChannelPool::releaseChannel(std::shared_ptr<QAbstractGrpcChannel> channel) {
    if (!channel) {
        return;
    }

    QMutexLocker locker(&mutex_);

    if (!initialized_) {
        return;
    }

    activeChannels_--;
    pool_.enqueue(channel);
}

std::shared_ptr<QAbstractGrpcChannel> GrpcChannelPool::createChannel() {
    QUrl url;
    url.setScheme("http");
    url.setHost(host_);
    url.setPort(port_);

    QGrpcChannelOptions options(url);
    options.withDeadline(std::chrono::milliseconds(timeoutConfig_.requestTimeoutMs));

    auto channel = std::make_shared<QGrpcHttp2Channel>(options);

    qDebug() << "[GrpcChannelPool] Created channel for" << host_ << ":" << port_;
    return channel;
}

void GrpcChannelPool::setRetryPolicy(const RetryPolicy& policy) {
    QMutexLocker locker(&mutex_);
    retryPolicy_ = policy;
}

void GrpcChannelPool::setTimeoutConfig(const TimeoutConfig& config) {
    QMutexLocker locker(&mutex_);
    timeoutConfig_ = config;
}
