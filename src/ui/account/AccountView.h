#ifndef HTS_VER6_ACCOUNTVIEW_H
#define HTS_VER6_ACCOUNTVIEW_H

#include "../widget/DomainWidget.h"

class QLabel;
class QTableWidget;
class AccountViewModel;

class AccountView : public DomainWidget {
    Q_OBJECT
public:
    explicit AccountView(QWidget* parent = nullptr);

    QString windowId() const override { return "account"; }
    QString windowTitle() const override { return tr("Account"); }
    bool isSingleton() const override { return true; }

    void setViewModel(AccountViewModel* viewModel);
    AccountViewModel* viewModel() const { return viewModel_; }

protected:
    QSize preferredSize() const override { return QSize(800, 600); }
    QSize minimumSizeHint() const override { return QSize(600, 450); }
    QSize maximumSizeHint() const override { return QSize(1200, 900); }

private slots:
    void onBalanceChanged(double newBalance);
    void onReservedChanged(double newReserved);
    void onAvailableChanged(double newAvailable);

private:
    void setupUI();
    void connectViewModel();
    QString formatCurrency(double amount) const;

    AccountViewModel* viewModel_;

    QLabel* accountNumberLabel;
    QLabel* totalAssetsLabel;
    QLabel* cashBalanceLabel;
    QLabel* cashReservedLabel;
    QLabel* cashAvailableLabel;
    QLabel* stockValueLabel;
    QLabel* profitLossLabel;
    QLabel* profitRateLabel;
    QTableWidget* positionsTable;
};

#endif
