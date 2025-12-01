#include "OrderCommandService.h"
#include "../../../infrastructure/network/GrpcNetworkManager.h"
#include "../../../infrastructure/session/SessionManager.h"
#include <QDebug>
#include <order_api.qpb.h>

// Helper to convert OrderSide to proto Side
order::SideGadget::Side toProtoSide(OrderSide side) {
    return side == OrderSide::Buy ? order::SideGadget::Side::BUY : order::SideGadget::Side::SELL;
}

// Helper to convert OrderType to proto OrderType
order::OrderTypeGadget::OrderType toProtoOrderType(OrderType type) {
    return type == OrderType::Market ? order::OrderTypeGadget::OrderType::MARKET : order::OrderTypeGadget::OrderType::LIMIT;
}

// Helper to convert TimeInForce to proto TimeInForce
order::TimeInForceGadget::TimeInForce toProtoTimeInForce(TimeInForce tif) {
    switch (tif) {
        case TimeInForce::GTC: return order::TimeInForceGadget::TimeInForce::GTC;
        case TimeInForce::IOC: return order::TimeInForceGadget::TimeInForce::IOC;
        case TimeInForce::FOK: return order::TimeInForceGadget::TimeInForce::FOK;
        case TimeInForce::Day:
        default: return order::TimeInForceGadget::TimeInForce::DAY;
    }
}

// Helper to convert proto OrderStatus to QString
QString orderStatusToString(order::OrderStatusGadget::OrderStatus status) {
    switch (status) {
        case order::OrderStatusGadget::OrderStatus::RECEIVED: return "RECEIVED";
        case order::OrderStatusGadget::OrderStatus::ACCEPTED: return "ACCEPTED";
        case order::OrderStatusGadget::OrderStatus::SENT: return "SENT";
        case order::OrderStatusGadget::OrderStatus::PARTIALLY_FILLED: return "PARTIALLY_FILLED";
        case order::OrderStatusGadget::OrderStatus::FILLED: return "FILLED";
        case order::OrderStatusGadget::OrderStatus::CANCEL_REQUESTED: return "CANCEL_REQUESTED";
        case order::OrderStatusGadget::OrderStatus::CANCELED: return "CANCELED";
        case order::OrderStatusGadget::OrderStatus::REJECTED: return "REJECTED";
        default: return "UNKNOWN";
    }
}

void OrderCommandService::placeOrderAsync(
    const PlaceOrderParams& params,
    QObject* context,
    std::function<void(Result<OrderResult>)> callback
) {
    QString sessionId = SessionManager::instance().currentSessionId();
    qint64 accountId = SessionManager::instance().currentAccountId();

    if (sessionId.isEmpty() || accountId == 0) {
        callback(Result<OrderResult>::failure(
            ErrorCode::InvalidInput,
            "No active session. Please login first."
        ));
        return;
    }

    QString sessionWithAccount = QString("%1:%2").arg(accountId).arg(sessionId);

    if (params.symbol.isEmpty()) {
        callback(Result<OrderResult>::failure(
            ErrorCode::InvalidInput,
            "Symbol cannot be empty"
        ));
        return;
    }

    if (params.quantity <= 0) {
        callback(Result<OrderResult>::failure(
            ErrorCode::InvalidInput,
            "Quantity must be greater than 0"
        ));
        return;
    }

    if (params.orderType == OrderType::Limit && params.price <= 0) {
        callback(Result<OrderResult>::failure(
            ErrorCode::InvalidInput,
            "Limit orders must have a price greater than 0"
        ));
        return;
    }

    order::PlaceOrderRequest request;
    request.setSessionId(sessionId);
    request.setSymbol(params.symbol);
    request.setSide(toProtoSide(params.side));
    request.setOrderType(toProtoOrderType(params.orderType));
    request.setQuantity(params.quantity);
    request.setPrice(params.orderType == OrderType::Market ? 0 : params.price);
    request.setTimeInForce(toProtoTimeInForce(params.timeInForce));

    auto& networkMgr = GrpcNetworkManager::instance();
    networkMgr.executeCallAsync<order::PlaceOrderRequest, order::OrderResponse>(
        networkMgr.orderClient(),
        request,
        [](QAbstractGrpcClient* client, const order::PlaceOrderRequest& req, const QGrpcCallOptions& opts) {
            return static_cast<order::OrderService::Client*>(client)->PlaceOrder(req, opts);
        },
        context,
        [callback](Result<order::OrderResponse> result) {
            if (result.isError()) {
                QString userMessage = QString("Failed to place order: %1").arg(result.error().message);
                callback(Result<OrderResult>::failure(result.error().code, userMessage));
                return;
            }

            auto& reply = result.value();

            if (reply.orderId() == 0 || reply.status() == order::OrderStatusGadget::OrderStatus::REJECTED) {
                QString rejectMessage = reply.message().isEmpty()
                    ? "Order was rejected by the server"
                    : reply.message();
                callback(Result<OrderResult>::failure(ErrorCode::InvalidInput, rejectMessage));
                return;
            }

            OrderResult orderResult;
            orderResult.orderId = reply.orderId();
            orderResult.status = orderStatusToString(reply.status());
            orderResult.message = reply.message();
            orderResult.timestamp = reply.timestamp();

            callback(Result<OrderResult>::success(std::move(orderResult)));
        }
    );
}

void OrderCommandService::cancelOrderAsync(
    const QString& symbol,
    int64_t orderId,
    QObject* context,
    std::function<void(Result<OrderResult>)> callback
) {
    QString sessionId = SessionManager::instance().currentSessionId();
    if (sessionId.isEmpty()) {
        callback(Result<OrderResult>::failure(
            ErrorCode::InvalidInput,
            "No active session. Please login first."
        ));
        return;
    }

    if (symbol.isEmpty()) {
        callback(Result<OrderResult>::failure(
            ErrorCode::InvalidInput,
            "Symbol cannot be empty"
        ));
        return;
    }

    if (orderId <= 0) {
        callback(Result<OrderResult>::failure(
            ErrorCode::InvalidInput,
            "Invalid order ID"
        ));
        return;
    }

    order::CancelOrderRequest request;
    request.setSessionId(sessionId);
    request.setSymbol(symbol);
    request.setOrderId(orderId);

    auto& networkMgr = GrpcNetworkManager::instance();
    networkMgr.executeCallAsync<order::CancelOrderRequest, order::OrderResponse>(
        networkMgr.orderClient(),
        request,
        [](QAbstractGrpcClient* client, const order::CancelOrderRequest& req, const QGrpcCallOptions& opts) {
            return static_cast<order::OrderService::Client*>(client)->CancelOrder(req, opts);
        },
        context,
        [callback](Result<order::OrderResponse> result) {
            if (result.isError()) {
                QString userMessage = QString("Failed to cancel order: %1").arg(result.error().message);
                callback(Result<OrderResult>::failure(result.error().code, userMessage));
                return;
            }

            auto& reply = result.value();

            if (reply.status() == order::OrderStatusGadget::OrderStatus::REJECTED) {
                QString rejectMessage = reply.message().isEmpty()
                    ? "Cancel request was rejected by the server"
                    : reply.message();
                callback(Result<OrderResult>::failure(ErrorCode::InvalidInput, rejectMessage));
                return;
            }

            OrderResult orderResult;
            orderResult.orderId = reply.orderId();
            orderResult.status = orderStatusToString(reply.status());
            orderResult.message = reply.message();
            orderResult.timestamp = reply.timestamp();

            callback(Result<OrderResult>::success(std::move(orderResult)));
        }
    );
}
