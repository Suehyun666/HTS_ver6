#ifndef HTS_VER6_ORDERVIEW_H
#define HTS_VER6_ORDERVIEW_H
#include "../widget/DomainWidget.h"
#include <memory>

class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;
class QPushButton;
class QTableWidget;
class QLabel;
class OrderViewModel;
struct OrderDisplayInfo;

class OrderWidget : public DomainWidget {
    Q_OBJECT
public:
    explicit OrderWidget(OrderViewModel* viewModel, QWidget* parent = nullptr);

    QString windowId() const override { return "order"; }
    QString windowTitle() const override { return tr("Order"); }

protected:
    QSize preferredSize() const override { return QSize(900, 700); }
    QSize minimumSizeHint() const override { return QSize(600, 500); }
    QSize maximumSizeHint() const override { return QSize(1400, 1000); }

private slots:
    void onBuyClicked();
    void onSellClicked();
    void onCancelClicked();
    void onOrderPlaced(const OrderDisplayInfo& order);
    void onOrderCancelled(qint64 orderId);
    void onOrderFailed(const QString& message);

private:
    void setupUI();
    void connectViewModel();
    void addOrderToTable(const OrderDisplayInfo& order);
    int findOrderRow(qint64 orderId);

    OrderViewModel* viewModel_;

    QLineEdit* symbolEdit;
    QDoubleSpinBox* priceSpinBox;
    QSpinBox* quantitySpinBox;
    QComboBox* orderTypeCombo;
    QPushButton* buyButton;
    QPushButton* sellButton;
    QTableWidget* ordersTable;
    QLabel* statusLabel;
};

#endif