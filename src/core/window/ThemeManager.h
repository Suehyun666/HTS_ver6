#ifndef HTS_VER6_THEMEMANAGER_H
#define HTS_VER6_THEMEMANAGER_H

#include <QObject>
#include <QString>
#include <QColor>

class ThemeManager : public QObject {
    Q_OBJECT
public:
    static ThemeManager& instance();

    enum class Theme {
        Light,
        Dark,
        System
    };

    void loadTheme(Theme theme);
    void loadThemeFromFile(const QString& filePath);
    void loadSystemTheme();
    void applyTheme();

    Theme currentTheme() const { return currentTheme_; }
    QString getStyleSheet() const;

    QColor backgroundColor() const { return backgroundColor_; }
    QColor foregroundColor() const { return foregroundColor_; }
    QColor accentColor() const { return accentColor_; }
    QColor borderColor() const { return borderColor_; }
    QColor buttonColor() const { return buttonColor_; }
    QColor buttonHoverColor() const { return buttonHoverColor_; }

signals:
    void themeChanged();

private:
    ThemeManager();
    Theme currentTheme_;

    QColor backgroundColor_;
    QColor foregroundColor_;
    QColor accentColor_;
    QColor borderColor_;
    QColor buttonColor_;
    QColor buttonHoverColor_;

    void loadLightTheme();
    void loadDarkTheme();
};

#endif
