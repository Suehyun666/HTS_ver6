#include "OrderViewModel.h"
#include <QDateTime>

OrderViewModel::OrderViewModel(IOrderCommandService* orderService, QObject* parent)
    : QObject(parent)
    , orderService(orderService)
    , symbol_("AAPL")
    , price_(150.00)
    , quantity_(100)
    , orderTypeIndex_(1)  // Default to Limit
    , busy_(false)
{
}

void OrderViewModel::setSymbol(const QString& symbol) {
    if (symbol_ != symbol) {
        symbol_ = symbol;
        emit symbolChanged();
    }
}

void OrderViewModel::setPrice(double price) {
    if (price_ != price) {
        price_ = price;
        emit priceChanged();
    }
}

void OrderViewModel::setQuantity(int quantity) {
    if (quantity_ != quantity) {
        quantity_ = quantity;
        emit quantityChanged();
    }
}

void OrderViewModel::setOrderTypeIndex(int index) {
    if (orderTypeIndex_ != index) {
        orderTypeIndex_ = index;
        emit orderTypeIndexChanged();
    }
}

void OrderViewModel::setBusy(bool busy) {
    if (busy_ != busy) {
        busy_ = busy;
        emit busyChanged();
    }
}

void OrderViewModel::setErrorMessage(const QString& msg) {
    if (errorMessage_ != msg) {
        errorMessage_ = msg;
        emit errorMessageChanged();
    }
}

void OrderViewModel::buyCommand() {
    placeOrder(OrderSide::Buy);
}

void OrderViewModel::sellCommand() {
    placeOrder(OrderSide::Sell);
}

void OrderViewModel::placeOrder(OrderSide side) {
    if (busy_) {
        return;
    }

    setBusy(true);
    setErrorMessage("");

    PlaceOrderParams params;
    params.symbol = symbol_;
    params.side = side;
    params.orderType = orderTypeIndex_ == 0 ? OrderType::Market : OrderType::Limit;
    params.quantity = quantity_;
    params.price = orderTypeIndex_ == 0 ? 0 : static_cast<int64_t>(price_ * 1'000'000);  // Convert to micro_units (1,000,000 = 1.0)
    params.timeInForce = TimeInForce::Day;

    orderService->placeOrderAsync(params, this, [this, side](Result<OrderResult> result) {
        setBusy(false);

        if (result.isError()) {
            setErrorMessage(result.error().message);
            emit orderFailed(result.error().message);
            return;
        }

        auto& orderResult = result.value();

        OrderDisplayInfo displayInfo;
        displayInfo.orderId = orderResult.orderId;
        displayInfo.symbol = symbol_;
        displayInfo.side = side == OrderSide::Buy ? "Buy" : "Sell";
        displayInfo.orderType = orderTypeIndex_ == 0 ? "Market" : "Limit";
        displayInfo.price = price_;
        displayInfo.quantity = quantity_;
        displayInfo.status = orderResult.status;
        displayInfo.timestamp = orderResult.timestamp;

        emit orderPlaced(displayInfo);
    });
}

void OrderViewModel::cancelOrderCommand(const QString& symbol, qint64 orderId) {
    if (busy_) {
        return;
    }

    setBusy(true);
    setErrorMessage("");

    orderService->cancelOrderAsync(symbol, orderId, this, [this, orderId](Result<OrderResult> result) {
        setBusy(false);

        if (result.isError()) {
            setErrorMessage(result.error().message);
            emit orderFailed(result.error().message);
            return;
        }

        emit orderCancelled(orderId);
    });
}
