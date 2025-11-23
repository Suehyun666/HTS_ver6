#include "Config.h"
#include "../../core/model/Constants.h"
#include <QFile>
#include <QSettings>

Config::Config()
    : grpcAuthServer_(AppConstants::DEFAULT_GRPC_AUTH_SERVER)
    , grpcTradingServer_(AppConstants::DEFAULT_GRPC_TRADING_SERVER)
    , streamServer_(AppConstants::DEFAULT_STREAM_SERVER)
    , maxGrpcChannels_(AppConstants::DEFAULT_MAX_GRPC_CHANNELS)
    , networkThreads_(AppConstants::DEFAULT_NETWORK_THREADS)
{
}

Config& Config::instance() {
    static Config inst;
    return inst;
}

Result<void> Config::load(const QString& configPath) {
    if (!QFile::exists(configPath)) {
        return Result<void>::success();
    }

    QSettings fileSettings(configPath, QSettings::IniFormat);
    grpcAuthServer_ = fileSettings.value("network/grpc_auth_server", grpcAuthServer_).toString();
    grpcTradingServer_ = fileSettings.value("network/grpc_trading_server", grpcTradingServer_).toString();
    streamServer_ = fileSettings.value("network/stream_server", streamServer_).toString();
    maxGrpcChannels_ = fileSettings.value("network/max_grpc_channels", maxGrpcChannels_).toInt();
    networkThreads_ = fileSettings.value("network/threads", networkThreads_).toInt();

    QSettings appSettings(AppConstants::ORGANIZATION_NAME, AppConstants::APP_NAME);

    if (!appSettings.contains(AppConstants::SETTINGS_THEME)) {
        QString theme = fileSettings.value("ui/theme", "light").toString();
        appSettings.setValue(AppConstants::SETTINGS_THEME, theme);
    }

    if (!appSettings.contains(AppConstants::SETTINGS_LANGUAGE)) {
        QString language = fileSettings.value("ui/language", AppConstants::DEFAULT_LANGUAGE).toString();
        appSettings.setValue(AppConstants::SETTINGS_LANGUAGE, language);
    }

    return Result<void>::success();
}
