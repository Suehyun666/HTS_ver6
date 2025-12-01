#include "OrderWidget.h"
#include "../../viewmodel/order/OrderViewModel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
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
#include <QDateTime>

OrderWidget::OrderWidget(OrderViewModel* viewModel, QWidget *parent)
    : DomainWidget(parent)
    , viewModel_(viewModel)
    , symbolEdit(nullptr)
    , priceSpinBox(nullptr)
    , quantitySpinBox(nullptr)
    , orderTypeCombo(nullptr)
    , buyButton(nullptr)
    , sellButton(nullptr)
    , ordersTable(nullptr)
    , statusLabel(nullptr)
{
    setupUI();
    connectViewModel();
}

void OrderWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    // Order Entry Section - Clean grid layout
    QWidget* orderEntryWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(orderEntryWidget);
    gridLayout->setContentsMargins(12, 12, 12, 12);
    gridLayout->setHorizontalSpacing(12);
    gridLayout->setVerticalSpacing(10);

    // Row 0: Symbol and Order Type
    QLabel* symbolLabel = new QLabel(tr("Symbol"));
    symbolLabel->setStyleSheet("font-weight: 500;");
    symbolEdit = new QLineEdit("AAPL");
    symbolEdit->setMinimumHeight(32);

    QLabel* typeLabel = new QLabel(tr("Type"));
    typeLabel->setStyleSheet("font-weight: 500;");
    orderTypeCombo = new QComboBox();
    orderTypeCombo->addItem(tr("Market"));
    orderTypeCombo->addItem(tr("Limit"));
    orderTypeCombo->setCurrentIndex(1);
    orderTypeCombo->setMinimumHeight(32);

    gridLayout->addWidget(symbolLabel, 0, 0);
    gridLayout->addWidget(symbolEdit, 0, 1);
    gridLayout->addWidget(typeLabel, 0, 2);
    gridLayout->addWidget(orderTypeCombo, 0, 3);

    // Row 1: Quantity and Price
    QLabel* qtyLabel = new QLabel(tr("Quantity"));
    qtyLabel->setStyleSheet("font-weight: 500;");
    quantitySpinBox = new QSpinBox();
    quantitySpinBox->setRange(1, 999999);
    quantitySpinBox->setValue(100);
    quantitySpinBox->setMinimumHeight(32);

    QLabel* priceLabel = new QLabel(tr("Price"));
    priceLabel->setStyleSheet("font-weight: 500;");
    priceSpinBox = new QDoubleSpinBox();
    priceSpinBox->setRange(0.01, 999999.99);
    priceSpinBox->setValue(150.00);
    priceSpinBox->setPrefix("$ ");
    priceSpinBox->setDecimals(2);
    priceSpinBox->setMinimumHeight(32);

    gridLayout->addWidget(qtyLabel, 1, 0);
    gridLayout->addWidget(quantitySpinBox, 1, 1);
    gridLayout->addWidget(priceLabel, 1, 2);
    gridLayout->addWidget(priceSpinBox, 1, 3);

    // Row 2: Action Buttons
    buyButton = new QPushButton(tr("BUY"));
    sellButton = new QPushButton(tr("SELL"));

    buyButton->setMinimumHeight(40);
    sellButton->setMinimumHeight(40);
    buyButton->setStyleSheet(
        "QPushButton { background-color: #10b981; color: white; font-weight: bold; "
        "border: none; border-radius: 4px; padding: 8px 24px; }"
        "QPushButton:hover { background-color: #059669; }"
        "QPushButton:pressed { background-color: #047857; }"
    );
    sellButton->setStyleSheet(
        "QPushButton { background-color: #ef4444; color: white; font-weight: bold; "
        "border: none; border-radius: 4px; padding: 8px 24px; }"
        "QPushButton:hover { background-color: #dc2626; }"
        "QPushButton:pressed { background-color: #b91c1c; }"
    );

    gridLayout->addWidget(buyButton, 2, 0, 1, 2);
    gridLayout->addWidget(sellButton, 2, 2, 1, 2);

    mainLayout->addWidget(orderEntryWidget);

    // Status Label
    statusLabel = new QLabel();
    statusLabel->setWordWrap(true);
    statusLabel->setMinimumHeight(24);
    mainLayout->addWidget(statusLabel);

    // Orders Table - Clean, minimal design
    QLabel* ordersHeader = new QLabel(tr("Open Orders"));
    ordersHeader->setStyleSheet("font-size: 14px; font-weight: 600; padding: 4px 0;");
    mainLayout->addWidget(ordersHeader);

    ordersTable = new QTableWidget();
    ordersTable->setColumnCount(7);
    ordersTable->setHorizontalHeaderLabels({
        tr("Order ID"), tr("Symbol"), tr("Side"), tr("Type"),
        tr("Price"), tr("Qty"), tr("Status")
    });

    ordersTable->horizontalHeader()->setStretchLastSection(true);
    ordersTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ordersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ordersTable->setAlternatingRowColors(true);
    ordersTable->setShowGrid(false);
    ordersTable->verticalHeader()->setVisible(false);
    ordersTable->setStyleSheet(
        "QTableWidget { border: none; }"
        "QHeaderView::section { background-color: transparent; border: none; "
        "border-bottom: 2px solid palette(mid); padding: 6px; font-weight: 600; }"
    );

    mainLayout->addWidget(ordersTable, 1);

    // Cancel Button
    QPushButton* cancelButton = new QPushButton(tr("Cancel Selected Order"));
    cancelButton->setMinimumHeight(36);
    mainLayout->addWidget(cancelButton);

    connect(buyButton, &QPushButton::clicked, this, &OrderWidget::onBuyClicked);
    connect(sellButton, &QPushButton::clicked, this, &OrderWidget::onSellClicked);
    connect(cancelButton, &QPushButton::clicked, this, &OrderWidget::onCancelClicked);
}

void OrderWidget::connectViewModel() {
    if (!viewModel_) return;

    // Connect ViewModel signals to Widget slots
    connect(viewModel_, &OrderViewModel::orderPlaced, this, &OrderWidget::onOrderPlaced);
    connect(viewModel_, &OrderViewModel::orderCancelled, this, &OrderWidget::onOrderCancelled);
    connect(viewModel_, &OrderViewModel::orderFailed, this, &OrderWidget::onOrderFailed);

    // Sync UI inputs with ViewModel
    connect(symbolEdit, &QLineEdit::textChanged, viewModel_, &OrderViewModel::setSymbol);
    connect(priceSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            viewModel_, &OrderViewModel::setPrice);
    connect(quantitySpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            viewModel_, &OrderViewModel::setQuantity);
    connect(orderTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            viewModel_, &OrderViewModel::setOrderTypeIndex);

    // Initialize UI with ViewModel values
    symbolEdit->setText(viewModel_->symbol());
    priceSpinBox->setValue(viewModel_->price());
    quantitySpinBox->setValue(viewModel_->quantity());
    orderTypeCombo->setCurrentIndex(viewModel_->orderTypeIndex());
}

void OrderWidget::onBuyClicked() {
    if (!viewModel_) return;
    statusLabel->setText(tr("Placing buy order..."));
    statusLabel->setStyleSheet("color: #3b82f6;");
    viewModel_->buyCommand();
}

void OrderWidget::onSellClicked() {
    if (!viewModel_) return;
    statusLabel->setText(tr("Placing sell order..."));
    statusLabel->setStyleSheet("color: #3b82f6;");
    viewModel_->sellCommand();
}

void OrderWidget::onCancelClicked() {
    if (!viewModel_) return;

    int row = ordersTable->currentRow();
    if (row < 0) {
        statusLabel->setText(tr("Please select an order to cancel"));
        statusLabel->setStyleSheet("color: #f59e0b;");
        return;
    }

    qint64 orderId = ordersTable->item(row, 0)->text().toLongLong();
    QString symbol = ordersTable->item(row, 1)->text();

    statusLabel->setText(tr("Cancelling order..."));
    statusLabel->setStyleSheet("color: #3b82f6;");
    viewModel_->cancelOrderCommand(symbol, orderId);
}

void OrderWidget::onOrderPlaced(const OrderDisplayInfo& order) {
    addOrderToTable(order);
    statusLabel->setText(tr("Order placed successfully: %1 %2 %3 @ $%4")
        .arg(order.side)
        .arg(order.quantity)
        .arg(order.symbol)
        .arg(order.price, 0, 'f', 2));
    statusLabel->setStyleSheet("color: #10b981;");
}

void OrderWidget::onOrderCancelled(qint64 orderId) {
    int row = findOrderRow(orderId);
    if (row >= 0) {
        ordersTable->removeRow(row);
        statusLabel->setText(tr("Order #%1 cancelled successfully").arg(orderId));
        statusLabel->setStyleSheet("color: #10b981;");
    }
}

void OrderWidget::onOrderFailed(const QString& message) {
    statusLabel->setText(tr("Error: %1").arg(message));
    statusLabel->setStyleSheet("color: #ef4444; font-weight: 500;");
}

void OrderWidget::addOrderToTable(const OrderDisplayInfo& order) {
    int row = ordersTable->rowCount();
    ordersTable->insertRow(row);

    ordersTable->setItem(row, 0, new QTableWidgetItem(QString::number(order.orderId)));
    ordersTable->setItem(row, 1, new QTableWidgetItem(order.symbol));

    auto* sideItem = new QTableWidgetItem(order.side);
    if (order.side == "Buy") {
        sideItem->setForeground(QBrush(QColor("#10b981")));
    } else {
        sideItem->setForeground(QBrush(QColor("#ef4444")));
    }
    ordersTable->setItem(row, 2, sideItem);

    ordersTable->setItem(row, 3, new QTableWidgetItem(order.orderType));
    ordersTable->setItem(row, 4, new QTableWidgetItem(QString("$%1").arg(order.price, 0, 'f', 2)));
    ordersTable->setItem(row, 5, new QTableWidgetItem(QString::number(order.quantity)));
    ordersTable->setItem(row, 6, new QTableWidgetItem(order.status));
}

int OrderWidget::findOrderRow(qint64 orderId) {
    for (int row = 0; row < ordersTable->rowCount(); ++row) {
        if (ordersTable->item(row, 0)->text().toLongLong() == orderId) {
            return row;
        }
    }
    return -1;
}