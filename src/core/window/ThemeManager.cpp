#include "ThemeManager.h"
#include "../model/Constants.h"
#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QPalette>
#include <QGuiApplication>

ThemeManager& ThemeManager::instance() {
    static ThemeManager instance;
    return instance;
}

ThemeManager::ThemeManager() : currentTheme_(Theme::Light) {
    QSettings settings(AppConstants::ORGANIZATION_NAME, AppConstants::APP_NAME);
    QString themeName = settings.value(AppConstants::SETTINGS_THEME, "light").toString();

    if (themeName == "dark") {
        loadTheme(Theme::Dark);
    } else {
        loadTheme(Theme::Light);
    }
}

void ThemeManager::loadTheme(Theme theme) {
    currentTheme_ = theme;

    if (theme == Theme::Dark) {
        loadDarkTheme();
    } else {
        loadLightTheme();
    }

    QSettings settings(AppConstants::ORGANIZATION_NAME, AppConstants::APP_NAME);
    settings.setValue(AppConstants::SETTINGS_THEME, theme == Theme::Dark ? "dark" : "light");

    applyTheme();
    emit themeChanged();
}

void ThemeManager::loadThemeFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isObject()) {
        return;
    }

    QJsonObject obj = doc.object();
    backgroundColor_ = QColor(obj["background"].toString());
    foregroundColor_ = QColor(obj["foreground"].toString());
    accentColor_ = QColor(obj["accent"].toString());
    borderColor_ = QColor(obj["border"].toString());
    buttonColor_ = QColor(obj["button"].toString());
    buttonHoverColor_ = QColor(obj["buttonHover"].toString());

    applyTheme();
    emit themeChanged();
}

void ThemeManager::loadLightTheme() {
    backgroundColor_ = QColor("#FFFFFF");
    foregroundColor_ = QColor("#000000");
    accentColor_ = QColor("#0078D4");
    borderColor_ = QColor("#CCCCCC");
    buttonColor_ = QColor("#E0E0E0");
    buttonHoverColor_ = QColor("#D0D0D0");
}

void ThemeManager::loadDarkTheme() {
    backgroundColor_ = QColor("#1E1E1E");
    foregroundColor_ = QColor("#FFFFFF");
    accentColor_ = QColor("#0078D4");
    borderColor_ = QColor("#3F3F3F");
    buttonColor_ = QColor("#2D2D2D");
    buttonHoverColor_ = QColor("#3E3E3E");
}

void ThemeManager::loadSystemTheme() {
    QPalette palette = QGuiApplication::palette();
    backgroundColor_ = palette.color(QPalette::Window);
    foregroundColor_ = palette.color(QPalette::WindowText);
    accentColor_ = palette.color(QPalette::Highlight);
    borderColor_ = palette.color(QPalette::Mid);
    buttonColor_ = palette.color(QPalette::Button);
    buttonHoverColor_ = palette.color(QPalette::Light);
}

void ThemeManager::applyTheme() {
    qApp->setStyleSheet(getStyleSheet());
}

QString ThemeManager::getStyleSheet() const {
    return QString(R"(
        QWidget {
            background-color: %1;
            color: %2;
        }
        QPushButton {
            background-color: %5;
            color: %2;
            border: 1px solid %4;
            padding: 5px 15px;
            border-radius: 3px;
        }
        QPushButton:hover {
            background-color: %6;
        }
        QPushButton:pressed {
            background-color: %4;
        }
        QLineEdit, QTextEdit {
            background-color: %1;
            color: %2;
            border: 1px solid %4;
            padding: 3px;
        }
        QMenuBar {
            background-color: %1;
            color: %2;
        }
        QMenuBar::item:selected {
            background-color: %3;
            color: white;
        }
        QMenu {
            background-color: %1;
            color: %2;
            border: 1px solid %4;
        }
        QMenu::item:selected {
            background-color: %3;
            color: white;
        }
        QCalendarWidget QWidget {
            background-color: %1;
            color: %2;
        }
        QCalendarWidget QAbstractItemView {
            background-color: %1;
            color: %2;
            selection-background-color: %3;
            selection-color: white;
        }
    )").arg(backgroundColor_.name())
       .arg(foregroundColor_.name())
       .arg(accentColor_.name())
       .arg(borderColor_.name())
       .arg(buttonColor_.name())
       .arg(buttonHoverColor_.name());
}
