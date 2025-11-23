#include "AccountView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QRandomGenerator>

AccountView::AccountView(QWidget* parent)
    : DomainWidget(parent)
    , accountNumberLabel(nullptr)
    , totalAssetsLabel(nullptr)
    , cashBalanceLabel(nullptr)
    , stockValueLabel(nullptr)
    , profitLossLabel(nullptr)
    , profitRateLabel(nullptr)
    , positionsTable(nullptr)
{
    setupUI();
    loadMockData();
}

void AccountView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QGroupBox* summaryGroup = new QGroupBox(tr("Account Summary"));
    QGridLayout* summaryLayout = new QGridLayout(summaryGroup);

    accountNumberLabel = new QLabel("123-456-7890");
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

    summaryLayout->addWidget(new QLabel(tr("Account Number:")), 0, 0);
    summaryLayout->addWidget(accountNumberLabel, 0, 1);
    summaryLayout->addWidget(new QLabel(tr("Total Assets:")), 1, 0);
    summaryLayout->addWidget(totalAssetsLabel, 1, 1);
    summaryLayout->addWidget(new QLabel(tr("Cash Balance:")), 2, 0);
    summaryLayout->addWidget(cashBalanceLabel, 2, 1);

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

void AccountView::loadMockData() {
    double cashBalance = 50000.0 + QRandomGenerator::global()->bounded(50000);
    double stockValue = 0.0;
    double totalPL = 0.0;

    QStringList symbols = {"AAPL", "GOOGL", "MSFT", "TSLA", "AMZN", "META", "NVDA", "AMD"};

    for (int i = 0; i < 6; ++i) {
        int row = positionsTable->rowCount();
        positionsTable->insertRow(row);

        QString symbol = symbols[i];
        int quantity = (QRandomGenerator::global()->bounded(20) + 1) * 10;
        double avgPrice = 50.0 + QRandomGenerator::global()->bounded(200);
        double currentPrice = avgPrice + (QRandomGenerator::global()->bounded(1000) / 1000.0 - 0.5) * 30;
        double value = quantity * currentPrice;
        double pl = (currentPrice - avgPrice) * quantity;
        double plPercent = ((currentPrice - avgPrice) / avgPrice) * 100;

        stockValue += value;
        totalPL += pl;

        positionsTable->setItem(row, 0, new QTableWidgetItem(symbol));
        positionsTable->setItem(row, 1, new QTableWidgetItem(QString::number(quantity)));
        positionsTable->setItem(row, 2, new QTableWidgetItem(QString("$%1").arg(avgPrice, 0, 'f', 2)));
        positionsTable->setItem(row, 3, new QTableWidgetItem(QString("$%1").arg(currentPrice, 0, 'f', 2)));
        positionsTable->setItem(row, 4, new QTableWidgetItem(QString("$%1").arg(value, 0, 'f', 2)));

        QTableWidgetItem* plItem = new QTableWidgetItem(QString("$%1").arg(pl, 0, 'f', 2));
        plItem->setForeground(pl >= 0 ? Qt::darkGreen : Qt::red);
        positionsTable->setItem(row, 5, plItem);

        QTableWidgetItem* plPercentItem = new QTableWidgetItem(QString("%1%").arg(plPercent, 0, 'f', 2));
        plPercentItem->setForeground(plPercent >= 0 ? Qt::darkGreen : Qt::red);
        positionsTable->setItem(row, 6, plPercentItem);
    }

    double totalAssets = cashBalance + stockValue;
    double profitRate = (totalPL / (totalAssets - totalPL)) * 100;

    cashBalanceLabel->setText(QString("$%1").arg(cashBalance, 0, 'f', 2));
    stockValueLabel->setText(QString("$%1").arg(stockValue, 0, 'f', 2));
    totalAssetsLabel->setText(QString("$%1").arg(totalAssets, 0, 'f', 2));
    profitLossLabel->setText(QString("$%1").arg(totalPL, 0, 'f', 2));
    profitLossLabel->setStyleSheet(totalPL >= 0 ? "color: green;" : "color: red;");
    profitRateLabel->setText(QString("%1%").arg(profitRate, 0, 'f', 2));
    profitRateLabel->setStyleSheet(profitRate >= 0 ? "color: green;" : "color: red;");
}
