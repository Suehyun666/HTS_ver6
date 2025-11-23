#pragma once
#include <QObject>
#include <QTranslator>
#include <QApplication>
#include <QLocale>
#include <QString>
#include <vector>
#include <memory>
#include "../../../core/model/Result.h"

class TranslationManager : public QObject {
    Q_OBJECT
public:
    static TranslationManager& instance();

    Result<void> initialize(QApplication* app);
    Result<void> loadLanguage(const QString& languageCode);

    QString currentLanguage() const { return currentLanguage_; }
    QStringList availableLanguages() const;

signals:
    void languageChanged(const QString& languageCode);

private:
    TranslationManager() = default;

    Result<void> loadDomainTranslation(const QString& domain, const QString& languageCode);
    void clearTranslations();

    QApplication* app_ = nullptr;
    QString currentLanguage_;

    std::vector<std::unique_ptr<QTranslator>> translators_;

    // 도메인별 번역 파일 목록
    const std::vector<QString> domains_ = {
        "common",
        "auth",
        "account",
        "order",
        "market"
    };
};
