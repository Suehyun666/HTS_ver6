#ifndef HTS_VER6_ORDERVIEW_H
#define HTS_VER6_ORDERVIEW_H
#include "../widget/DomainWidget.h"

class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;
class QPushButton;
class QTableWidget;

class OrderWidget : public DomainWidget {
    Q_OBJECT
public:
    explicit OrderWidget(QWidget* parent = nullptr);

    QString windowId() const override { return "order"; }
    QString windowTitle() const override { return tr("Order"); }

protected:
    QSize preferredSize() const override { return QSize(700, 600); }
    QSize minimumSizeHint() const override { return QSize(500, 450); }
    QSize maximumSizeHint() const override { return QSize(1000, 900); }

private slots:
    void onBuyClicked();
    void onSellClicked();
    void onCancelClicked();

private:
    void setupUI();
    void addMockOrders();

    QLineEdit* symbolEdit;
    QDoubleSpinBox* priceSpinBox;
    QSpinBox* quantitySpinBox;
    QComboBox* orderTypeCombo;
    QPushButton* buyButton;
    QPushButton* sellButton;
    QTableWidget* ordersTable;
};

#endif