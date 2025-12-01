#pragma once
#include "IOrderCommandService.h"

class OrderCommandService : public IOrderCommandService {
public:
    OrderCommandService() = default;

    void placeOrderAsync(
        const PlaceOrderParams& params,
        QObject* context,
        std::function<void(Result<OrderResult>)> callback
    ) override;

    void cancelOrderAsync(
        const QString& symbol,
        int64_t orderId,
        QObject* context,
        std::function<void(Result<OrderResult>)> callback
    ) override;
};
