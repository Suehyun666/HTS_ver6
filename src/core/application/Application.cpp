#include "Application.h"
#include "../model/Constants.h"
#include "../window/ThemeManager.h"
#include "../../infrastructure/config/Config.h"
#include "../../infrastructure/network/NetworkExecutor.h"
#include "../../infrastructure/network/GrpcChannelPool.h"
#include "../../infrastructure/network/GrpcNetworkManager.h"
#include "../../infrastructure/session/SessionManager.h"
#include "../../infrastructure/database/OHLCVRepository.h"
#include "../../infrastructure/config/translate/TranslationManager.h"
#include "../../infrastructure/streaming/StreamingClient.h"
#include "../../domain/service/auth/AuthCommandService.h"
#include "../../viewmodel/auth/LoginViewModel.h"
#include "../../viewmodel/account/AccountManager.h"
#include "../../ui/window/MainWindow.h"
#include "../../ui/auth/LoginWindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QTimer>

Application::Application(int& argc, char** argv)
    : app(argc, argv)
{
    app.setApplicationName(AppConstants::APP_NAME);
    app.setApplicationVersion(AppConstants::APP_VERSION);
    app.setOrganizationName(AppConstants::ORGANIZATION_NAME);
    app.setOrganizationDomain(AppConstants::ORGANIZATION_DOMAIN);
}

Application::~Application() {
    shutdown();
}

Result<void> Application::initialize() {
    qDebug() << "[Application] Starting initialization sequence...";

    // Step 1: Load configuration
    auto configResult = loadConfig();
    if (configResult.isError()) {
        qCritical() << "[Application] Config load failed:" << configResult.error().message;
        return configResult;
    }
    qDebug() << "[Application] Config loaded successfully";

    // Step 2: Initialize translations
    auto translationResult = TranslationManager::instance().initialize(&app);
    if (translationResult.isError()) {
        qWarning() << "[Application] Translation init failed:" << translationResult.error().message;
    } else {
        qDebug() << "[Application] Translations loaded successfully";
    }

    // Step 2.5: Initialize theme (must be after config load)
    ThemeManager::instance();
    qDebug() << "[Application] Theme initialized";

    // Step 3: Initialize infrastructure
    auto infraResult = initializeInfrastructure();
    if (infraResult.isError()) {
        qCritical() << "[Application] Infrastructure init failed:" << infraResult.error().message;
        rollbackInfrastructure();
        return infraResult;
    }
    qDebug() << "[Application] Infrastructure initialized successfully";

    // Step 3: Initialize memory pools (placeholder)
    auto memResult = initializeMemoryPools();
    if (memResult.isError()) {
        qCritical() << "[Application] Memory pool init failed:" << memResult.error().message;
        rollbackInfrastructure();
        return memResult;
    }
    qDebug() << "[Application] Memory pools initialized successfully";

    // Step 4: Initialize network layer
    auto netResult = initializeNetwork();
    if (netResult.isError()) {
        qCritical() << "[Application] Network init failed:" << netResult.error().message;
        rollbackNetwork();
        rollbackInfrastructure();
        return netResult;
    }
    qDebug() << "[Application] Network initialized successfully";

    // Step 5: Initialize views
    auto viewResult = initializeViews();
    if (viewResult.isError()) {
        qCritical() << "[Application] View init failed:" << viewResult.error().message;
        rollbackNetwork();
        rollbackInfrastructure();
        return viewResult;
    }
    qDebug() << "[Application] Views initialized successfully";

    qDebug() << "[Application] Initialization complete";
    return Result<void>::success();
}

int Application::run() {
    if (!loginWindow) {
        qCritical() << "[Application] Cannot run: views not initialized";
        return -1;
    }

    loginWindow->show();
    return app.exec();
}

void Application::shutdown() {
    qDebug() << "[Application] Starting shutdown sequence...";

    shouldReconnect = false;

    if (streamingClient) {
        streamingClient->disconnect();
    }

    rollbackNetwork();
    rollbackInfrastructure();

    AccountManager::destroy();

    qDebug() << "[Application] Shutdown complete";
}

Result<void> Application::loadConfig() {
    QString configPath = QCoreApplication::applicationDirPath() + "/../../config.ini";
    Config::instance().load(configPath);
    configLoaded = true;
    return Result<void>::success();
}

Result<void> Application::initializeInfrastructure() {
    if (!OHLCVRepository::instance().initialize()) {
        return Result<void>::failure(ErrorCode::ConfigLoadFailed, "OHLCV database initialization failed");
    }
    qDebug() << "[Application] OHLCV database initialized";

    infrastructureInitialized = true;
    return Result<void>::success();
}

Result<void> Application::initializeMemoryPools() {
    // TODO: 실제 메모리 풀 구현
    // 지금은 placeholder
    qDebug() << "[Application] Memory pools: using default allocator";
    return Result<void>::success();
}

Result<void> Application::initializeNetwork() {
    auto& config = Config::instance();

    auto result = NetworkExecutor::instance().initialize(config.networkThreads());
    if (result.isError()) {
        return result;
    }

    auto poolResult = GrpcChannelPool::instance().initialize("localhost", 50051);
    if (poolResult.isError()) {
        NetworkExecutor::instance().shutdown();
        return poolResult;
    }

    RetryPolicy retryPolicy;
    retryPolicy.maxRetries = 3;
    retryPolicy.initialBackoffMs = 100;
    retryPolicy.maxBackoffMs = 5000;
    retryPolicy.backoffMultiplier = 2.0;
    GrpcChannelPool::instance().setRetryPolicy(retryPolicy);

    TimeoutConfig timeoutConfig;
    timeoutConfig.connectTimeoutMs = 5000;
    timeoutConfig.requestTimeoutMs = 10000;
    GrpcChannelPool::instance().setTimeoutConfig(timeoutConfig);

    auto networkMgrResult = GrpcNetworkManager::instance().initialize();
    if (networkMgrResult.isError()) {
        GrpcChannelPool::instance().shutdown();
        NetworkExecutor::instance().shutdown();
        return networkMgrResult;
    }

    // Initialize OHLCV database
    bool dbInitSuccess = OHLCVRepository::instance().initialize();
    if (!dbInitSuccess) {
        qWarning() << "[Database] Failed to initialize OHLCV database, continuing anyway...";
    }

    networkInitialized = true;
    return Result<void>::success();
}

Result<void> Application::initializeViews() {
    auto authService = std::make_unique<AuthCommandService>();
    auto loginViewModel = new LoginViewModel(std::move(authService), nullptr);

    loginWindow = new LoginWindow(loginViewModel, nullptr);
    loginWindow->setAttribute(Qt::WA_DeleteOnClose);

    QObject::connect(loginWindow, &LoginWindow::loginSucceeded,
                     loginWindow, [this]() { onLoginSucceeded(); });

    return Result<void>::success();
}

void Application::onLoginSucceeded() {
    qDebug() << "[Application] Login succeeded, connecting to streaming server...";

    // Connect to streaming server FIRST before showing main window
    streamingClient = new StreamingClient(&app);

    // Initialize AccountManager singleton to handle account data
    AccountManager::initialize(streamingClient);

    QObject::connect(streamingClient, &StreamingClient::connected, [this]() {
        qDebug() << "[Application] StreamingClient connected, authenticating...";
        QString sessionId = SessionManager::instance().currentSessionId();
        streamingClient->authenticate(sessionId);
    });

    QObject::connect(streamingClient, &StreamingClient::authenticated,
                    [this](qint64 userId, QList<qint64> accountIds) {
        qDebug() << "[Application] StreamingClient authenticated. userId:" << userId
                 << "accounts:" << accountIds;

        // Only NOW show the main window after successful streaming connection
        mainWindow = new MainWindow(nullptr);
        mainWindow->setAttribute(Qt::WA_DeleteOnClose);
        mainWindow->setWindowTitle(AppConstants::APP_NAME);
        mainWindow->resize(AppConstants::MAIN_WINDOW_WIDTH,
                            AppConstants::MAIN_WINDOW_HEIGHT);
        mainWindow->show();

        if (loginWindow) {
            loginWindow->close();
            loginWindow->deleteLater();
            loginWindow = nullptr;
        }

        qDebug() << "[Application] Main window displayed, login window deleted";
    });

    QObject::connect(streamingClient, &StreamingClient::authenticationFailed,
                    [this](const QString& reason) {
        qCritical() << "[Application] StreamingClient auth failed:" << reason;

        int ret = QMessageBox::critical(nullptr, "Authentication Failed",
                              "Streaming server authentication failed: " + reason +
                              "\n\nWould you like to retry?",
                              QMessageBox::Retry | QMessageBox::Close);

        if (ret == QMessageBox::Retry) {
            qDebug() << "[Application] Retrying streaming connection...";
            QTimer::singleShot(2000, [this]() {
                QString wsUrl = "ws://localhost:9050/ws";
                streamingClient->connectToServer(wsUrl);
            });
        } else {
            app.quit();
        }
    });

    QObject::connect(streamingClient, &StreamingClient::disconnected,
                    [this]() {
        qWarning() << "[Application] StreamingClient disconnected";

        if (!shouldReconnect) {
            qDebug() << "[Application] Reconnection disabled, not attempting to reconnect";
            return;
        }

        qDebug() << "[Application] Attempting reconnect in 3 seconds...";
        QTimer::singleShot(3000, [this]() {
            if (streamingClient && shouldReconnect) {
                qDebug() << "[Application] Reconnecting to streaming server...";
                QString wsUrl = "ws://localhost:9050/ws";
                streamingClient->connectToServer(wsUrl);
            }
        });
    });

    QObject::connect(streamingClient, &StreamingClient::errorOccurred,
                    [this](const QString& error) {
        qCritical() << "[Application] StreamingClient error:" << error;

        // Don't quit immediately on error, let reconnection logic handle it
        qDebug() << "[Application] Streaming error occurred, waiting for reconnection...";
    });

    QString wsUrl = "ws://localhost:9050/ws";
    qDebug() << "[Application] Connecting to streaming server:" << wsUrl;
    streamingClient->connectToServer(wsUrl);
}

void Application::rollbackInfrastructure() {
    if (infrastructureInitialized) {
        SessionManager::instance().clear();
        infrastructureInitialized = false;
        qDebug() << "[Application] Infrastructure rollback complete";
    }
}

void Application::rollbackNetwork() {
    if (networkInitialized) {
        GrpcNetworkManager::instance().shutdown();
        GrpcChannelPool::instance().shutdown();
        NetworkExecutor::instance().shutdown();
        networkInitialized = false;
        qDebug() << "[Application] Network rollback complete";
    }
}
