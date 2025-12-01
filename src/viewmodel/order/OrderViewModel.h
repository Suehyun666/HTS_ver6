#pragma once
#include <QObject>
#include <QString>
#include "../../domain/service/order/IOrderCommandService.h"

struct OrderDisplayInfo {
    int64_t orderId;
    QString symbol;
    QString side;
    QString orderType;
    double price;
    int64_t quantity;
    QString status;
    int64_t timestamp;
};

class OrderViewModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString symbol READ symbol WRITE setSymbol NOTIFY symbolChanged)
    Q_PROPERTY(double price READ price WRITE setPrice NOTIFY priceChanged)
    Q_PROPERTY(int quantity READ quantity WRITE setQuantity NOTIFY quantityChanged)
    Q_PROPERTY(int orderTypeIndex READ orderTypeIndex WRITE setOrderTypeIndex NOTIFY orderTypeIndexChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    explicit OrderViewModel(IOrderCommandService* orderService, QObject* parent = nullptr);

    QString symbol() const { return symbol_; }
    double price() const { return price_; }
    int quantity() const { return quantity_; }
    int orderTypeIndex() const { return orderTypeIndex_; }
    bool busy() const { return busy_; }
    QString errorMessage() const { return errorMessage_; }

    void setSymbol(const QString& symbol);
    void setPrice(double price);
    void setQuantity(int quantity);
    void setOrderTypeIndex(int index);

    Q_INVOKABLE void buyCommand();
    Q_INVOKABLE void sellCommand();
    Q_INVOKABLE void cancelOrderCommand(const QString& symbol, qint64 orderId);

signals:
    void symbolChanged();
    void priceChanged();
    void quantityChanged();
    void orderTypeIndexChanged();
    void busyChanged();
    void errorMessageChanged();

    void orderPlaced(const OrderDisplayInfo& order);
    void orderCancelled(qint64 orderId);
    void orderFailed(const QString& message);

private:
    void placeOrder(OrderSide side);
    void setBusy(bool busy);
    void setErrorMessage(const QString& msg);

    IOrderCommandService* orderService;

    QString symbol_;
    double price_;
    int quantity_;
    int orderTypeIndex_;  // 0=Market, 1=Limit
    bool busy_;
    QString errorMessage_;
};
