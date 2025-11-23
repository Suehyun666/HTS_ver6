#pragma once
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
class QMdiArea;
QT_END_NAMESPACE

class WindowManager;
class MenuBarManager;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void centerOnScreen();

    Ui::MainWindow* ui;
    QMdiArea* mdiArea;
    WindowManager* windowManager;
    MenuBarManager* menuBarManager;
};
