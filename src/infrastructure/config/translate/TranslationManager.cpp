#include "TranslationManager.h"
#include "../../../core/model/Constants.h"
#include <QDir>
#include <QDebug>
#include <QSettings>

TranslationManager& TranslationManager::instance() {
    static TranslationManager inst;
    return inst;
}

Result<void> TranslationManager::initialize(QApplication* app) {
    if (!app) {
        return Result<void>::failure(
            ErrorCode::InvalidInput,
            "QApplication pointer is null"
        );
    }

    app_ = app;

    // 저장된 언어 설정 로드, 없으면 시스템 로케일 또는 기본 언어
    QSettings settings(AppConstants::ORGANIZATION_NAME, AppConstants::APP_NAME);
    QString language = settings.value(AppConstants::SETTINGS_LANGUAGE, "").toString();

    if (language.isEmpty()) {
        language = QLocale::system().name(); // e.g., "ko_KR", "en_US"
        if (!language.contains("ko_KR") && !language.contains("en_US")) {
            language = AppConstants::DEFAULT_LANGUAGE;
        }
    }

    return loadLanguage(language);
}

Result<void> TranslationManager::loadLanguage(const QString& languageCode) {
    if (!app_) {
        return Result<void>::failure(
            ErrorCode::Unknown,
            "TranslationManager not initialized"
        );
    }

    qDebug() << "[TranslationManager] Loading language:" << languageCode;

    // 기존 번역 제거
    clearTranslations();

    // 각 도메인별 번역 파일 로드
    for (const auto& domain : domains_) {
        auto result = loadDomainTranslation(domain, languageCode);
        if (result.isError()) {
            qWarning() << "[TranslationManager] Failed to load domain:" << domain
                       << "Error:" << result.error().message;
            // 실패해도 계속 진행 (일부 번역 파일 누락 허용)
        }
    }

    currentLanguage_ = languageCode;

    // 설정 저장
    QSettings settings(AppConstants::ORGANIZATION_NAME, AppConstants::APP_NAME);
    settings.setValue(AppConstants::SETTINGS_LANGUAGE, languageCode);

    qDebug() << "[TranslationManager] Language loaded successfully:" << languageCode;

    // 언어 변경 시그널 발생
    emit languageChanged(languageCode);

    return Result<void>::success();
}

QStringList TranslationManager::availableLanguages() const {
    return {"ko_KR", "en_US"};
}

Result<void> TranslationManager::loadDomainTranslation(const QString& domain, const QString& languageCode) {
    // 번역 파일 경로: resources/i18n/{domain}_{language}.qm
    QString fileName = QString("%1_%2.qm").arg(domain).arg(languageCode);
    QString resourcePath = QString(":/resources/i18n/%1").arg(fileName);

    auto translator = std::make_unique<QTranslator>();

    // 먼저 리소스에서 로드 시도
    bool loaded = translator->load(resourcePath);

    // 리소스에 없으면 파일 시스템에서 로드 시도
    if (!loaded) {
        QDir resourceDir(QCoreApplication::applicationDirPath() + "/../resources/i18n");
        if (!resourceDir.exists()) {
            resourceDir.setPath("resources/i18n");
        }

        QString absolutePath = resourceDir.absoluteFilePath(fileName);
        loaded = translator->load(fileName.replace(".qm", ""), resourceDir.absolutePath());

        if (!loaded) {
            return Result<void>::failure(
                ErrorCode::ConfigLoadFailed,
                QString("Failed to load translation file: %1 (tried resource: %2, file: %3)")
                    .arg(fileName).arg(resourcePath).arg(absolutePath)
            );
        }
    }

    if (!app_->installTranslator(translator.get())) {
        return Result<void>::failure(
            ErrorCode::Unknown,
            QString("Failed to install translator: %1").arg(domain)
        );
    }

    qDebug() << "[TranslationManager] Loaded translation:" << domain << languageCode << "from" << (loaded ? "resource" : "file");

    translators_.push_back(std::move(translator));

    return Result<void>::success();
}

void TranslationManager::clearTranslations() {
    for (auto& translator : translators_) {
        app_->removeTranslator(translator.get());
    }
    translators_.clear();
}
