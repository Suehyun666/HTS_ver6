#include "DomainWidget.h"
#include "../../core/window/ThemeManager.h"
#include "../../infrastructure/config/translate/TranslationManager.h"

DomainWidget::DomainWidget(QWidget* parent) : QWidget(parent) {
    setupSizeConstraints();

    // Theme change signal
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &DomainWidget::onThemeChanged);
    applyTheme();

    // Language change signal
    connect(&TranslationManager::instance(), &TranslationManager::languageChanged,
            this, &DomainWidget::onLanguageChanged);
}

void DomainWidget::saveState(QJsonObject& state) {
}

void DomainWidget::loadState(const QJsonObject& state) {
}

void DomainWidget::onActivated() {
}

void DomainWidget::onDeactivated() {
}

bool DomainWidget::canClose() const {
    return true;
}

void DomainWidget::setupSizeConstraints() {
    setMinimumSize(minimumSizeHint());
    setMaximumSize(maximumSizeHint());
    resize(preferredSize());
}

void DomainWidget::applyTheme() {
    // Theme is applied globally via ThemeManager
    // Individual widgets don't need to override
}

void DomainWidget::onThemeChanged() {
    applyTheme();
}

void DomainWidget::retranslateUi() {
    // Base implementation does nothing
    // Subclasses should override this to update their UI text
}

void DomainWidget::onLanguageChanged(const QString& languageCode) {
    Q_UNUSED(languageCode);
    retranslateUi();
}
