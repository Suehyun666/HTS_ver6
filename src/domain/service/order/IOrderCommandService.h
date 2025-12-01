#pragma once
#include "core/model/Result.h"
#include <QString>
#include <QObject>
#include <functional>

enum class OrderSide {
    Buy,
    Sell
};

enum class OrderType {
    Market,
    Limit
};

enum class TimeInForce {
    Day,
    GTC,  // Good Till Cancel
    IOC,  // Immediate or Cancel
    FOK   // Fill or Kill
};

struct PlaceOrderParams {
    QString symbol;
    OrderSide side;
    OrderType orderType;
    int64_t quantity;
    int64_t price;  // micro_units (1,000,000 = 1.0), 0 for market orders
    TimeInForce timeInForce;
};

struct OrderResult {
    int64_t orderId;
    QString status;
    QString message;
    int64_t timestamp;
};

class IOrderCommandService {
public:
    virtual ~IOrderCommandService() = default;

    virtual void placeOrderAsync(
        const PlaceOrderParams& params,
        QObject* context,
        std::function<void(Result<OrderResult>)> callback
    ) = 0;

    virtual void cancelOrderAsync(
        const QString& symbol,
        int64_t orderId,
        QObject* context,
        std::function<void(Result<OrderResult>)> callback
    ) = 0;
};
