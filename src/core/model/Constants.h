#pragma once

namespace AppConstants {
    // Application
    constexpr const char* APP_NAME = "HTS ver6";
    constexpr const char* APP_VERSION = "0.1.0";
    constexpr const char* ORGANIZATION_NAME = "Your Company";
    constexpr const char* ORGANIZATION_DOMAIN = "yourcompany.com";

    // Configuration
    constexpr const char* CONFIG_FILE_NAME = "config.ini";
    constexpr const char* DEFAULT_LANGUAGE = "ko_KR";

    // Network Defaults
    constexpr const char* DEFAULT_GRPC_AUTH_SERVER = "localhost:50051";
    constexpr const char* DEFAULT_GRPC_TRADING_SERVER = "localhost:50052";
    constexpr const char* DEFAULT_STREAM_SERVER = "localhost:50056";
    constexpr int DEFAULT_MAX_GRPC_CHANNELS = 4;
    constexpr int DEFAULT_NETWORK_THREADS = 2;

    // Timeouts (milliseconds)
    constexpr int GRPC_TIMEOUT_MS = 5000;
    constexpr int STREAM_CONNECT_TIMEOUT_MS = 10000;
    constexpr int SESSION_EXPIRE_SECONDS = 3600;

    // UI Defaults - MainWindow
    constexpr int MIN_WINDOW_WIDTH = 800;
    constexpr int MIN_WINDOW_HEIGHT = 600;
    constexpr int MAIN_WINDOW_WIDTH = 1400;
    constexpr int MAIN_WINDOW_HEIGHT = 900;

    // UI Defaults - LoginView
    constexpr int LOGIN_WINDOW_WIDTH = 400;
    constexpr int LOGIN_WINDOW_HEIGHT = 300;

    // Resource Paths
    constexpr const char* ICON_APP = ":/resources/icons/app_icon.png";
    constexpr const char* ICON_TRAY = ":/resources/icons/tray_icon.png";

    constexpr const char* SOUND_NOTIFICATION = ":/resources/sounds/notification.wav";
    constexpr const char* SOUND_ALERT = ":/resources/sounds/alert.wav";
    constexpr const char* SOUND_INFO = ":/resources/sounds/info.wav";

    constexpr const char* THEME_DARK = ":/resources/themes/dark.json";
    constexpr const char* THEME_LIGHT = ":/resources/themes/light.json";

    // QSettings Keys
    constexpr const char* SETTINGS_LANGUAGE = "settings/language";
    constexpr const char* SETTINGS_THEME = "settings/theme";
}

namespace ValidationRules {
    constexpr int MIN_USER_ID_LENGTH = 4;
    constexpr int MAX_USER_ID_LENGTH = 20;
    constexpr int MIN_PASSWORD_LENGTH = 6;
    constexpr int MAX_PASSWORD_LENGTH = 50;
}
