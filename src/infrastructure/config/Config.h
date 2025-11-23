#pragma once
#include <QString>
#include <QSettings>
#include "../../core/model/Result.h"

class Config {
public:
    static Config& instance();

    Result<void> load(const QString& configPath);

    QString grpcAuthServer() const { return grpcAuthServer_; }
    QString grpcTradingServer() const { return grpcTradingServer_; }
    QString streamServer() const { return streamServer_; }

    int maxGrpcChannels() const { return maxGrpcChannels_; }
    int networkThreads() const { return networkThreads_; }

private:
    Config();

    QString grpcAuthServer_;
    QString grpcTradingServer_;
    QString streamServer_;
    int maxGrpcChannels_;
    int networkThreads_;
};
