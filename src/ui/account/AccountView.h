#ifndef HTS_VER6_ACCOUNTVIEW_H
#define HTS_VER6_ACCOUNTVIEW_H

#include "../widget/DomainWidget.h"

class QLabel;
class QTableWidget;

class AccountView : public DomainWidget {
    Q_OBJECT
public:
    explicit AccountView(QWidget* parent = nullptr);

    QString windowId() const override { return "account"; }
    QString windowTitle() const override { return tr("Account"); }
    bool isSingleton() const override { return true; }

protected:
    QSize preferredSize() const override { return QSize(800, 600); }
    QSize minimumSizeHint() const override { return QSize(600, 450); }
    QSize maximumSizeHint() const override { return QSize(1200, 900); }

private:
    void setupUI();
    void loadMockData();

    QLabel* accountNumberLabel;
    QLabel* totalAssetsLabel;
    QLabel* cashBalanceLabel;
    QLabel* stockValueLabel;
    QLabel* profitLossLabel;
    QLabel* profitRateLabel;
    QTableWidget* positionsTable;
};

#endif
