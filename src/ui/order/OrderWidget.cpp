#include "OrderWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QRandomGenerator>

OrderWidget::OrderWidget(QWidget *parent)
    : DomainWidget(parent)
    , symbolEdit(nullptr)
    , priceSpinBox(nullptr)
    , quantitySpinBox(nullptr)
    , orderTypeCombo(nullptr)
    , buyButton(nullptr)
    , sellButton(nullptr)
    , ordersTable(nullptr)
{
    setupUI();
    addMockOrders();
}

void OrderWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QGroupBox* orderGroup = new QGroupBox(tr("New Order"));
    QFormLayout* formLayout = new QFormLayout(orderGroup);

    symbolEdit = new QLineEdit("AAPL");
    priceSpinBox = new QDoubleSpinBox();
    priceSpinBox->setRange(0.01, 999999.99);
    priceSpinBox->setValue(150.00);
    priceSpinBox->setPrefix("$ ");
    priceSpinBox->setDecimals(2);

    quantitySpinBox = new QSpinBox();
    quantitySpinBox->setRange(1, 999999);
    quantitySpinBox->setValue(100);

    orderTypeCombo = new QComboBox();
    orderTypeCombo->addItem(tr("Market Price"));
    orderTypeCombo->addItem(tr("Limit Price"));
    orderTypeCombo->setCurrentIndex(1);

    formLayout->addRow(tr("Symbol:"), symbolEdit);
    formLayout->addRow(tr("Order Type:"), orderTypeCombo);
    formLayout->addRow(tr("Price:"), priceSpinBox);
    formLayout->addRow(tr("Quantity:"), quantitySpinBox);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buyButton = new QPushButton(tr("Buy"));
    sellButton = new QPushButton(tr("Sell"));
    buyButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; }");
    sellButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-weight: bold; padding: 8px; }");

    buttonLayout->addWidget(buyButton);
    buttonLayout->addWidget(sellButton);
    formLayout->addRow(buttonLayout);

    mainLayout->addWidget(orderGroup);

    QGroupBox* ordersGroup = new QGroupBox(tr("Open Orders"));
    QVBoxLayout* ordersLayout = new QVBoxLayout(ordersGroup);

    ordersTable = new QTableWidget();
    ordersTable->setColumnCount(6);
    ordersTable->setHorizontalHeaderLabels({tr("Symbol"), tr("Type"), tr("Side"), tr("Price"), tr("Quantity"), tr("Status")});
    ordersTable->horizontalHeader()->setStretchLastSection(true);
    ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QPushButton* cancelButton = new QPushButton(tr("Cancel Selected"));
    ordersLayout->addWidget(ordersTable);
    ordersLayout->addWidget(cancelButton);

    mainLayout->addWidget(ordersGroup);

    connect(buyButton, &QPushButton::clicked, this, &OrderWidget::onBuyClicked);
    connect(sellButton, &QPushButton::clicked, this, &OrderWidget::onSellClicked);
    connect(cancelButton, &QPushButton::clicked, this, &OrderWidget::onCancelClicked);
}

void OrderWidget::addMockOrders() {
    QStringList symbols = {"AAPL", "GOOGL", "MSFT", "TSLA", "AMZN"};
    QStringList types = {tr("Limit"), tr("Market")};
    QStringList sides = {tr("Buy"), tr("Sell")};
    QStringList statuses = {tr("Pending"), tr("Filled"), tr("Partial")};

    for (int i = 0; i < 5; ++i) {
        int row = ordersTable->rowCount();
        ordersTable->insertRow(row);

        ordersTable->setItem(row, 0, new QTableWidgetItem(symbols[i]));
        ordersTable->setItem(row, 1, new QTableWidgetItem(types[i % 2]));
        ordersTable->setItem(row, 2, new QTableWidgetItem(sides[i % 2]));

        double price = 100.0 + QRandomGenerator::global()->bounded(200);
        ordersTable->setItem(row, 3, new QTableWidgetItem(QString("$%1").arg(price, 0, 'f', 2)));

        int qty = (QRandomGenerator::global()->bounded(10) + 1) * 10;
        ordersTable->setItem(row, 4, new QTableWidgetItem(QString::number(qty)));
        ordersTable->setItem(row, 5, new QTableWidgetItem(statuses[i % 3]));
    }
}

void OrderWidget::onBuyClicked() {
    QString symbol = symbolEdit->text().trimmed().toUpper();
    double price = priceSpinBox->value();
    int quantity = quantitySpinBox->value();
    QString orderType = orderTypeCombo->currentText();

    int row = ordersTable->rowCount();
    ordersTable->insertRow(row);
    ordersTable->setItem(row, 0, new QTableWidgetItem(symbol));
    ordersTable->setItem(row, 1, new QTableWidgetItem(orderType));
    ordersTable->setItem(row, 2, new QTableWidgetItem(tr("Buy")));
    ordersTable->setItem(row, 3, new QTableWidgetItem(QString("$%1").arg(price, 0, 'f', 2)));
    ordersTable->setItem(row, 4, new QTableWidgetItem(QString::number(quantity)));
    ordersTable->setItem(row, 5, new QTableWidgetItem(tr("Pending")));

    QMessageBox::information(this, tr("Order Placed"), tr("Buy order for %1 shares of %2 at $%3").arg(quantity).arg(symbol).arg(price, 0, 'f', 2));
}

void OrderWidget::onSellClicked() {
    QString symbol = symbolEdit->text().trimmed().toUpper();
    double price = priceSpinBox->value();
    int quantity = quantitySpinBox->value();
    QString orderType = orderTypeCombo->currentText();

    int row = ordersTable->rowCount();
    ordersTable->insertRow(row);
    ordersTable->setItem(row, 0, new QTableWidgetItem(symbol));
    ordersTable->setItem(row, 1, new QTableWidgetItem(orderType));
    ordersTable->setItem(row, 2, new QTableWidgetItem(tr("Sell")));
    ordersTable->setItem(row, 3, new QTableWidgetItem(QString("$%1").arg(price, 0, 'f', 2)));
    ordersTable->setItem(row, 4, new QTableWidgetItem(QString::number(quantity)));
    ordersTable->setItem(row, 5, new QTableWidgetItem(tr("Pending")));

    QMessageBox::information(this, tr("Order Placed"), tr("Sell order for %1 shares of %2 at $%3").arg(quantity).arg(symbol).arg(price, 0, 'f', 2));
}

void OrderWidget::onCancelClicked() {
    QList<QTableWidgetItem*> selected = ordersTable->selectedItems();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, tr("No Selection"), tr("Please select an order to cancel"));
        return;
    }

    int row = ordersTable->currentRow();
    if (row >= 0) {
        ordersTable->removeRow(row);
        QMessageBox::information(this, tr("Order Cancelled"), tr("Order has been cancelled"));
    }
}