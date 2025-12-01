#pragma once
#include "../model/Result.h"
#include <QApplication>

class LoginWindow;
class MainWindow;
class StreamingClient;
class AccountManager;

class Application {
public:
    explicit Application(int& argc, char** argv);
    ~Application();

    Result<void> initialize();
    int run();
    void shutdown();

private:
    Result<void> loadConfig();
    Result<void> initializeInfrastructure();
    Result<void> initializeMemoryPools();
    Result<void> initializeNetwork();
    Result<void> initializeViews();

    void rollbackInfrastructure();
    void rollbackNetwork();

    void onLoginSucceeded();

    QApplication app;
    LoginWindow* loginWindow = nullptr;
    MainWindow* mainWindow = nullptr;
    StreamingClient* streamingClient = nullptr;

    bool configLoaded = false;
    bool infrastructureInitialized = false;
    bool networkInitialized = false;
    bool shouldReconnect = true;
};
