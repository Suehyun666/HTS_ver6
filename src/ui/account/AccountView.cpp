#include "AccountView.h"
#include "../../viewmodel/account/AccountViewModel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QRandomGenerator>
#include <QDebug>

AccountView::AccountView(QWidget* parent)
    : DomainWidget(parent)
    , viewModel_(nullptr)
    , accountNumberLabel(nullptr)
    , totalAssetsLabel(nullptr)
    , cashBalanceLabel(nullptr)
    , cashReservedLabel(nullptr)
    , cashAvailableLabel(nullptr)
    , stockValueLabel(nullptr)
    , profitLossLabel(nullptr)
    , profitRateLabel(nullptr)
    , positionsTable(nullptr)
{
    setupUI();
}

void AccountView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QGroupBox* summaryGroup = new QGroupBox(tr("Account Summary"));
    QGridLayout* summaryLayout = new QGridLayout(summaryGroup);

    accountNumberLabel = new QLabel("-");
    totalAssetsLabel = new QLabel("$0.00");
    cashBalanceLabel = new QLabel("$0.00");
    stockValueLabel = new QLabel("$0.00");
    profitLossLabel = new QLabel("$0.00");
    profitRateLabel = new QLabel("0.00%");

    QFont valueFont;
    valueFont.setBold(true);
    valueFont.setPointSize(12);
    totalAssetsLabel->setFont(valueFont);
    profitLossLabel->setFont(valueFont);
    profitRateLabel->setFont(valueFont);

    cashReservedLabel = new QLabel("$0.00");
    cashAvailableLabel = new QLabel("$0.00");

    summaryLayout->addWidget(new QLabel(tr("Account Number:")), 0, 0);
    summaryLayout->addWidget(accountNumberLabel, 0, 1);
    summaryLayout->addWidget(new QLabel(tr("Total Assets:")), 1, 0);
    summaryLayout->addWidget(totalAssetsLabel, 1, 1);
    summaryLayout->addWidget(new QLabel(tr("Cash Balance:")), 2, 0);
    summaryLayout->addWidget(cashBalanceLabel, 2, 1);
    summaryLayout->addWidget(new QLabel(tr("Cash Reserved:")), 3, 0);
    summaryLayout->addWidget(cashReservedLabel, 3, 1);
    summaryLayout->addWidget(new QLabel(tr("Cash Available:")), 4, 0);
    summaryLayout->addWidget(cashAvailableLabel, 4, 1);

    summaryLayout->addWidget(new QLabel(tr("Stock Valuation:")), 1, 2);
    summaryLayout->addWidget(stockValueLabel, 1, 3);
    summaryLayout->addWidget(new QLabel(tr("Profit/Loss:")), 2, 2);
    summaryLayout->addWidget(profitLossLabel, 2, 3);
    summaryLayout->addWidget(new QLabel(tr("Profit Rate:")), 2, 4);
    summaryLayout->addWidget(profitRateLabel, 2, 5);

    mainLayout->addWidget(summaryGroup);

    QGroupBox* positionsGroup = new QGroupBox(tr("Holdings"));
    QVBoxLayout* positionsLayout = new QVBoxLayout(positionsGroup);

    positionsTable = new QTableWidget();
    positionsTable->setColumnCount(7);
    positionsTable->setHorizontalHeaderLabels({
        tr("Symbol"),
        tr("Quantity"),
        tr("Avg Price"),
        tr("Current Price"),
        tr("Value"),
        tr("P/L"),
        tr("P/L %")
    });
    positionsTable->horizontalHeader()->setStretchLastSection(true);
    positionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    positionsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    positionsTable->setAlternatingRowColors(true);

    positionsLayout->addWidget(positionsTable);
    mainLayout->addWidget(positionsGroup);
}

void AccountView::setViewModel(AccountViewModel* viewModel) {
    if (viewModel_ == viewModel) {
        qDebug() << "[AccountView] setViewModel called but same viewModel, skipping";
        return;
    }

    if (viewModel_) {
        disconnect(viewModel_, nullptr, this, nullptr);
    }

    viewModel_ = viewModel;

    if (viewModel_) {
        qDebug() << "[AccountView] Setting ViewModel - accountId:" << viewModel_->accountId()
                 << "balance:" << viewModel_->cashBalance()
                 << "reserved:" << viewModel_->cashReserved();

        connectViewModel();

        // Update UI with current values
        accountNumberLabel->setText(QString::number(viewModel_->accountId()));
        onBalanceChanged(viewModel_->cashBalance());
        onReservedChanged(viewModel_->cashReserved());
        onAvailableChanged(viewModel_->cashAvailable());
    } else {
        qWarning() << "[AccountView] setViewModel called with null viewModel!";
    }
}

void AccountView::connectViewModel() {
    if (!viewModel_) {
        qWarning() << "[AccountView::connectViewModel] viewModel is null!";
        return;
    }

    qDebug() << "[AccountView::connectViewModel] Connecting signals for accountId:" << viewModel_->accountId();

    bool connected1 = connect(viewModel_, &AccountViewModel::balanceChanged, this, &AccountView::onBalanceChanged);
    bool connected2 = connect(viewModel_, &AccountViewModel::reservedChanged, this, &AccountView::onReservedChanged);
    bool connected3 = connect(viewModel_, &AccountViewModel::availableChanged, this, &AccountView::onAvailableChanged);

    qDebug() << "[AccountView::connectViewModel] Connection results - balance:" << connected1
             << "reserved:" << connected2 << "available:" << connected3;
}

void AccountView::onBalanceChanged(double newBalance) {
    qDebug() << "[AccountView::onBalanceChanged] Called with:" << newBalance;
    QString formattedText = formatCurrency(newBalance);
    cashBalanceLabel->setText(formattedText);
    qDebug() << "[AccountView::onBalanceChanged] Label text set to:" << formattedText;
    cashBalanceLabel->update();
}

void AccountView::onReservedChanged(double newReserved) {
    qDebug() << "[AccountView::onReservedChanged] Called with:" << newReserved;
    QString formattedText = formatCurrency(newReserved);
    cashReservedLabel->setText(formattedText);
    qDebug() << "[AccountView::onReservedChanged] Label text set to:" << formattedText;
    cashReservedLabel->update();
}

void AccountView::onAvailableChanged(double newAvailable) {
    qDebug() << "[AccountView::onAvailableChanged] Called with:" << newAvailable;
    QString formattedText = formatCurrency(newAvailable);
    cashAvailableLabel->setText(formattedText);
    qDebug() << "[AccountView::onAvailableChanged] Label text set to:" << formattedText;

    QFont font = cashAvailableLabel->font();
    font.setBold(true);
    cashAvailableLabel->setFont(font);
    cashAvailableLabel->update();
}

QString AccountView::formatCurrency(double amount) const {
    return QString("$%1").arg(amount, 0, 'f', 2);
}
