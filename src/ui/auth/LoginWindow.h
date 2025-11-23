#pragma once
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
    class LoginWindow;
}
QT_END_NAMESPACE

class LoginViewModel;

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(LoginViewModel* viewModel, QWidget* parent = nullptr);
    ~LoginWindow() override;

signals:
    void loginSucceeded();

private slots:
    void onLoginClicked();
    void onBusyChanged();
    void onErrorMessageChanged();
    void onLoginSucceeded();
    void onLoginFailed(const QString& reason);

private:
    void connectSignals();
    void centerOnScreen();

    LoginViewModel* viewModel;
    Ui::LoginWindow* ui;
};
