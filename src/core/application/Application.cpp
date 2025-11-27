#include "Application.h"
#include "../model/Constants.h"
#include "../window/ThemeManager.h"
#include "../../infrastructure/config/Config.h"
#include "../../infrastructure/network/NetworkExecutor.h"
#include "../../infrastructure/session/SessionManager.h"
#include "../../infrastructure/config/translate/TranslationManager.h"
#include "../../domain/service/auth/AuthCommandService.h"
#include "../../viewmodel/auth/LoginViewModel.h"
#include "../../ui/window/MainWindow.h"
#include "../../ui/auth/LoginWindow.h"
#include <QMessageBox>
#include <QDebug>

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

    rollbackNetwork();
    rollbackInfrastructure();

    qDebug() << "[Application] Shutdown complete";
}

Result<void> Application::loadConfig() {
    QString configPath = QCoreApplication::applicationDirPath() + "/../../config.ini";
    Config::instance().load(configPath);
    configLoaded = true;
    return Result<void>::success();
}

Result<void> Application::initializeInfrastructure() {
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

    // NetworkExecutor 초기화
    auto result = NetworkExecutor::instance().initialize(config.networkThreads());
    if (result.isError()) {
        return result;
    }

    // TODO: GrpcChannelPool 초기화
    // TODO: StreamClient 초기화 (로그인 후에 연결)

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
    qDebug() << "[Application] Login succeeded, transitioning to main window";

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

    // TODO: StreamClient 연결 시작
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
        NetworkExecutor::instance().shutdown();
        // TODO: GrpcChannelPool shutdown
        // TODO: StreamClient shutdown
        networkInitialized = false;
        qDebug() << "[Application] Network rollback complete";
    }
}
