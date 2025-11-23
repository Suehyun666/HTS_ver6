#include "core/application/Application.h"
#include <QMessageBox>
#include <QDebug>

int main(int argc, char *argv[])
{
    // 1. Application 생성
    Application app(argc, argv);

    // 2. 초기화 시퀀스 실행
    auto initResult = app.initialize();
    if (initResult.isError()) {
        qCritical() << "Failed to initialize application:" << initResult.error().message;

        // GUI로 에러 표시 (QApplication이 생성되어 있으므로 가능)
        QMessageBox::critical(
            nullptr,
            "Initialization Failed",
            QString("Application failed to start:\n\n%1\n\nError Code: %2")
                .arg(initResult.error().message)
                .arg(static_cast<int>(initResult.error().code))
        );

        return static_cast<int>(initResult.error().code);
    }

    qDebug() << "Application initialized successfully, starting event loop...";

    // 3. Event loop 실행
    int exitCode = app.run();

    qDebug() << "Application exiting with code:" << exitCode;

    // 4. 명시적 정리 (소멸자에서도 호출되지만 명확성을 위해)
    app.shutdown();

    return exitCode;
}
