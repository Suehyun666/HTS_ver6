#include "LoginWindow.h"
#include "ui_loginwindow.h"
#include "../../viewmodel/auth/LoginViewModel.h"
#include "../../core/model/Constants.h"
#include <QScreen>
#include <QGuiApplication>

LoginWindow::LoginWindow(LoginViewModel* viewModel, QWidget* parent)
    : QWidget(parent)
    , viewModel(viewModel)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    resize(AppConstants::LOGIN_WINDOW_WIDTH, AppConstants::LOGIN_WINDOW_HEIGHT);
    connectSignals();
    centerOnScreen();
}

LoginWindow::~LoginWindow() {
    delete ui;
}

void LoginWindow::connectSignals() {
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);

    connect(viewModel, &LoginViewModel::busyChanged, this, &LoginWindow::onBusyChanged);
    connect(viewModel, &LoginViewModel::errorMessageChanged, this, &LoginWindow::onErrorMessageChanged);
    connect(viewModel, &LoginViewModel::loginSucceeded, this, &LoginWindow::onLoginSucceeded);
    connect(viewModel, &LoginViewModel::loginFailed, this, &LoginWindow::onLoginFailed);

    connect(ui->userIdEdit, &QLineEdit::textChanged, viewModel, &LoginViewModel::setUserId);
    connect(ui->passwordEdit, &QLineEdit::textChanged, viewModel, &LoginViewModel::setPassword);
}

void LoginWindow::centerOnScreen() {
    if (QScreen* screen = QGuiApplication::primaryScreen()) {
        QRect screenGeometry = screen->availableGeometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }
}

void LoginWindow::onLoginClicked() {
    viewModel->loginCommand();
}

void LoginWindow::onBusyChanged() {
    bool busy = viewModel->busy();
    ui->loginButton->setEnabled(!busy);
    ui->statusLabel->setText(busy ? tr("Logging in...") : "");
}

void LoginWindow::onErrorMessageChanged() {
    ui->statusLabel->setText(viewModel->errorMessage());
}

void LoginWindow::onLoginSucceeded() {
    emit loginSucceeded();
}

void LoginWindow::onLoginFailed(const QString& reason) {
    // 에러는 이미 errorMessage로 표시됨
    Q_UNUSED(reason);
}
