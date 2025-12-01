#include "StreamingClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

StreamingClient::StreamingClient(QObject* parent)
    : QObject(parent)
    , state_(StreamingState::Disconnected)
{
    connect(&webSocket_, &QWebSocket::connected, this, &StreamingClient::onConnected);
    connect(&webSocket_, &QWebSocket::disconnected, this, &StreamingClient::onDisconnected);
    connect(&webSocket_, &QWebSocket::textMessageReceived, this, &StreamingClient::onTextMessageReceived);
    connect(&webSocket_, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::errorOccurred),
            this, &StreamingClient::onError);
}

StreamingClient::~StreamingClient() {
    disconnect();
}

void StreamingClient::connectToServer(const QString& url) {
    if (state_ != StreamingState::Disconnected) {
        qWarning() << "[StreamingClient] Already connected or connecting";
        return;
    }

    qDebug() << "[StreamingClient] Connecting to:" << url;
    setState(StreamingState::Connecting);
    webSocket_.open(QUrl(url));
}

void StreamingClient::disconnect() {
    if (state_ != StreamingState::Disconnected) {
        qDebug() << "[StreamingClient] Disconnecting";
        webSocket_.close();
        setState(StreamingState::Disconnected);
    }
}

void StreamingClient::authenticate(const QString& sessionId) {
    if (state_ != StreamingState::Connected) {
        qWarning() << "[StreamingClient] Not connected, cannot authenticate";
        return;
    }

    sessionId_ = sessionId;

    QJsonObject authMsg;
    authMsg["type"] = "AUTH";
    authMsg["sessionId"] = sessionId;

    qDebug() << "[StreamingClient] Sending AUTH message with sessionId:" << sessionId;
    sendMessage(authMsg);
}

void StreamingClient::onConnected() {
    qDebug() << "[StreamingClient] WebSocket connected";
    setState(StreamingState::Connected);
    emit connected();
}

void StreamingClient::onDisconnected() {
    qDebug() << "[StreamingClient] WebSocket disconnected";
    setState(StreamingState::Disconnected);
    emit disconnected();
}

void StreamingClient::onTextMessageReceived(const QString& message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) {
        qWarning() << "[StreamingClient] Received invalid JSON:" << message;
        return;
    }

    QJsonObject msg = doc.object();
    handleMessage(msg);
}

void StreamingClient::onError(QAbstractSocket::SocketError error) {
    QString errorString = webSocket_.errorString();
    qCritical() << "[StreamingClient] WebSocket error:" << error << errorString;
    setState(StreamingState::Error);
    emit errorOccurred(errorString);
}

void StreamingClient::setState(StreamingState newState) {
    if (state_ != newState) {
        state_ = newState;
        emit stateChanged(newState);
    }
}

void StreamingClient::handleMessage(const QJsonObject& msg) {
    QString type = msg["type"].toString();

    if (type == "AUTH_OK") {
        handleAuthOk(msg);
    } else if (type == "AUTH_ERROR") {
        QString reason = msg["message"].toString("Authentication failed");
        qWarning() << "[StreamingClient] AUTH_ERROR:" << reason;
        emit authenticationFailed(reason);
        setState(StreamingState::Error);
    } else if (type == "ERROR") {
        QString code = msg["code"].toString();
        QString errorMsg = msg["message"].toString("Unknown error");
        qCritical() << "[StreamingClient] ERROR:" << code << "-" << errorMsg;
        emit errorOccurred(QString("%1: %2").arg(code, errorMsg));
        setState(StreamingState::Error);
    } else if (type == "ACCOUNT_SNAPSHOT") {
        // Handle account snapshot
        QJsonObject payload = msg["payload"].toObject();
        AccountSnapshot snapshot;
        snapshot.accountId = payload["accountId"].toInteger();
        snapshot.cashBalance = payload["balance"].toDouble();
        snapshot.cashReserved = payload["reserved"].toDouble();

        qDebug() << "[StreamingClient] Account snapshot received:" << snapshot.accountId;
        emit accountSnapshotReceived(snapshot);
    } else if (type == "ACCOUNT_EVENT") {
        handleAccountEvent(msg);
    } else if (type == "ORDER_FILL") {
        handleOrderFill(msg);
    } else {
        qDebug() << "[StreamingClient] Unknown message type:" << type;
    }
}

void StreamingClient::handleAuthOk(const QJsonObject& msg) {
    qint64 userId = msg["userId"].toInteger();
    QJsonArray accountsArray = msg["accounts"].toArray();

    QList<qint64> accountIds;
    for (const QJsonValue& val : accountsArray) {
        accountIds.append(val.toInteger());
    }

    qDebug() << "[StreamingClient] Authenticated successfully. userId:" << userId
             << "accounts:" << accountIds;

    setState(StreamingState::Authenticated);
    emit authenticated(userId, accountIds);
}

void StreamingClient::handleAccountEvent(const QJsonObject& msg) {
    QJsonObject payload = msg["payload"].toObject();
    qDebug() << "[StreamingClient] Account event received";
    emit accountEventReceived(payload);
}

void StreamingClient::handleOrderFill(const QJsonObject& msg) {
    QJsonObject payload = msg["payload"].toObject();
    qDebug() << "[StreamingClient] Order fill received";
    emit orderFillReceived(payload);
}

void StreamingClient::sendMessage(const QJsonObject& msg) {
    QJsonDocument doc(msg);
    QString jsonStr = doc.toJson(QJsonDocument::Compact);
    webSocket_.sendTextMessage(jsonStr);
}
