cmake_minimum_required(VERSION 3.16)

project(HTS_ver6 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_BINARY_DIR}/generated)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Qml Multimedia PrintSupport Grpc Protobuf Sql WebSockets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Qml Multimedia PrintSupport Grpc Protobuf Sql WebSockets)

set(TS_FILES
        resources/i18n/common_ko_KR.ts
        resources/i18n/auth_ko_KR.ts
        resources/i18n/account_ko_KR.ts
        resources/i18n/order_ko_KR.ts
        resources/i18n/market_ko_KR.ts
        resources/i18n/common_en_US.ts
        resources/i18n/auth_en_US.ts
        resources/i18n/account_en_US.ts
        resources/i18n/order_en_US.ts
        resources/i18n/market_en_US.ts
)

set(CORE_SOURCES
        src/core/model/Result.h
        src/core/model/Constants.h
        src/core/application/Application.h
        src/core/application/Application.cpp
        src/core/window/WindowManager.cpp
        src/core/window/WindowManager.h
        src/core/window/ThemeManager.cpp
        src/core/window/ThemeManager.h
        src/core/window/WorkspaceManager.cpp
        src/core/window/WorkspaceManager.h
        src/core/window/MenuBarManager.h
        src/core/window/MenuBarManager.cpp
        src/core/notification/NotificationManager.h
        src/core/notification/NotificationManager.cpp
        src/core/factory/ViewModelFactory.h
        src/core/factory/ViewModelFactory.cpp
)

set(DOMAIN_SOURCES
        src/domain/model/Session.h
        src/domain/model/AccountSnapshot.h
        src/domain/model/MarketTick.h
        src/domain/model/OHLCV.h
)

set(INFRASTRUCTURE_SOURCES
        src/infrastructure/config/Config.h
        src/infrastructure/config/Config.cpp
        src/infrastructure/config/translate/TranslationManager.h
        src/infrastructure/config/translate/TranslationManager.cpp
        src/infrastructure/network/NetworkExecutor.h
        src/infrastructure/network/NetworkExecutor.cpp
        src/infrastructure/network/GrpcChannelPool.h
        src/infrastructure/network/GrpcChannelPool.cpp
        src/infrastructure/network/GrpcNetworkManager.h
        src/infrastructure/network/GrpcNetworkManager.cpp
        src/infrastructure/session/SessionManager.h
        src/infrastructure/session/SessionManager.cpp
        src/infrastructure/repository/DataBase.cpp
        src/infrastructure/repository/DataBase.h
        src/infrastructure/cache/InMemoryCache.cpp
        src/infrastructure/cache/InMemoryCache.h
        src/infrastructure/database/OHLCVRepository.h
        src/infrastructure/database/OHLCVRepository.cpp
        src/infrastructure/streaming/StreamingClient.h
        src/infrastructure/streaming/StreamingClient.cpp
)

set(APPLICATION_SOURCES
        src/domain/service/auth/IAuthCommandService.h
        src/domain/service/auth/AuthCommandService.h
        src/domain/service/auth/AuthCommandService.cpp
        src/domain/service/account/IAccountQueryService.h
        src/domain/service/account/AccountQueryService.h
        src/domain/service/account/AccountQueryService.cpp
        src/domain/service/market/IMarketDataService.h
        src/domain/service/market/MarketDataService.cpp
        src/domain/service/market/MarketDataService.h
        src/domain/service/order/IOrderCommandService.h
        src/domain/service/order/OrderCommandService.cpp
        src/domain/service/order/OrderCommandService.h
        src/domain/service/chart/IChartDataService.h
        src/domain/service/chart/ChartDataService.h
        src/domain/service/chart/ChartDataService.cpp
)

set(VIEWMODEL_SOURCES
        src/viewmodel/auth/LoginViewModel.h
        src/viewmodel/auth/LoginViewModel.cpp
        src/viewmodel/account/PositionViewModel.cpp
        src/viewmodel/account/PositionViewModel.h
        src/viewmodel/account/BalanceViewModel.cpp
        src/viewmodel/account/BalanceViewModel.h
        src/viewmodel/account/AccountViewModel.h
        src/viewmodel/account/AccountViewModel.cpp
        src/viewmodel/account/AccountManager.h
        src/viewmodel/account/AccountManager.cpp
        src/viewmodel/order/OrderHistoryViewModel.cpp
        src/viewmodel/order/OrderHistoryViewModel.h
        src/viewmodel/order/OrderViewModel.h
        src/viewmodel/order/OrderViewModel.cpp
        src/viewmodel/chart/ChartViewModel.h
        src/viewmodel/chart/ChartViewModel.cpp
)

set(UI_SOURCES
        src/ui/window/MainWindow.h
        src/ui/window/MainWindow.cpp
        src/ui/auth/LoginWindow.cpp
        src/ui/auth/LoginWindow.cpp
        src/ui/order/OrderWidget.cpp
        src/ui/order/OrderWidget.h
        src/ui/account/AccountView.cpp
        src/ui/account/AccountView.h
        src/ui/widget/DomainWidget.h
        src/ui/widget/DomainWidget.cpp
        src/ui/chart/ChartWidget.cpp
        src/ui/chart/ChartWidget.h
)

set(MENU_SOURCES
        src/core/menu/OrderMenu.cpp
        src/core/menu/OrderMenu.h
        src/core/menu/AccountMenu.cpp
        src/core/menu/AccountMenu.h
        src/core/menu/ToolMenu.cpp
        src/core/menu/ToolMenu.h
        src/core/menu/MarketMenu.cpp
        src/core/menu/MarketMenu.h
)

set(TOOL_SOURCES
        src/ui/tools/calculator/StockCalculator.cpp
        src/ui/tools/calculator/StockCalculator.h
        src/ui/tools/calculator/CalculatorWidget.h
        src/ui/tools/calculator/CalculatorWidget.cpp
        src/ui/tools/calendar/calendar.cpp
        src/ui/tools/calendar/calendar.h
        src/ui/tools/calendar/CalendarWidget.h
        src/ui/tools/calendar/CalendarWidget.cpp
        src/ui/tools/note/NoteWidget.cpp
        src/ui/tools/note/NoteWidget.h
        src/ui/tools/clock/DigitalClock.cpp
        src/ui/tools/clock/DigitalClock.h
        src/ui/tools/clock/ClockWidget.h
        src/ui/tools/clock/ClockWidget.cpp
)

set(PLOT_SOURCES
        src/ui/plot/qcustomplot.cpp
        src/ui/plot/qcustomplot.h
)

set(UI_FILES
        src/ui/window/mainwindow.ui
        src/ui/auth/loginwindow.ui
)

set(PROTO_FILES
        resources/proto/auth_api.proto
        resources/proto/order_api.proto
        resources/proto/order_commons.proto
)

set(GRPC_PROTO_FILES
        resources/proto/auth_api.proto
        resources/proto/order_api.proto
)

set(PROJECT_SOURCES
        src/main.cpp
        ${CORE_SOURCES}
        ${DOMAIN_SOURCES}
        ${INFRASTRUCTURE_SOURCES}
        ${APPLICATION_SOURCES}
        ${VIEWMODEL_SOURCES}
        ${UI_SOURCES}
        ${MENU_SOURCES}
        ${TOOL_SOURCES}
        ${PLOT_SOURCES}
        src/viewmodel/order/OrderViewModel.cpp
        src/viewmodel/order/OrderViewModel.h
)

# Source groups for IDE
source_group("Core" FILES ${CORE_SOURCES})
source_group("Domain" FILES ${DOMAIN_SOURCES})
source_group("Infrastructure" FILES ${INFRASTRUCTURE_SOURCES})
source_group("Application" FILES ${APPLICATION_SOURCES})
source_group("ViewModel" FILES ${VIEWMODEL_SOURCES})
source_group("UI" FILES ${UI_SOURCES})
source_group("UI Files" FILES ${UI_FILES})
source_group("Menu" FILES ${MENU_SOURCES})
source_group("TOOL" FILES ${TOOL_SOURCES})
source_group("Plot" FILES ${PLOT_SOURCES})
source_group("Translation Files" FILES ${TS_FILES})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

    qt_add_executable(HTS_ver6
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            ${UI_FILES}
            ${TS_FILES}
            resources.qrc
            ${QM_FILES}
        )

    qt_add_protobuf(HTS_ver6
            PROTO_FILES ${PROTO_FILES}
            OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/generated
    )

    qt_add_grpc(HTS_ver6 CLIENT
            PROTO_FILES ${GRPC_PROTO_FILES}
            OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/generated
    )
else()
    if(ANDROID)
        add_library(HTS_ver6 SHARED
                ${PROJECT_SOURCES}
                resources.qrc
        )
    else()
        add_executable(HTS_ver6
            ${PROJECT_SOURCES}
            ${UI_FILES}
            ${TS_FILES}
            resources.qrc
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(HTS_ver6 PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Qml
        Qt${QT_VERSION_MAJOR}::Multimedia
        Qt${QT_VERSION_MAJOR}::PrintSupport
        Qt${QT_VERSION_MAJOR}::Grpc
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Protobuf
        Qt${QT_VERSION_MAJOR}::WebSockets)

# Windows-specific settings
if(WIN32)
    # Ensure proper Unicode support on Windows
    add_definitions(-DUNICODE -D_UNICODE)
endif()

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.HTS_ver6)
endif()

set_target_properties(HTS_ver6 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS HTS_ver6
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(HTS_ver6)
endif()
