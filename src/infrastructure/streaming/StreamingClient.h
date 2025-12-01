#pragma once

#include <QObject>
#include <QWebSocket>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <functional>

enum class StreamingState {
    Disconnected,
    Connecting,
    Connected,
    Authenticated,
    Error
};

struct AccountSnapshot {
    qint64 accountId;
    double cashBalance;
    double cashReserved;
    // Position info can be added later if needed
};

class StreamingClient : public QObject {
    Q_OBJECT

public:
    explicit StreamingClient(QObject* parent = nullptr);
    ~StreamingClient();

    void connectToServer(const QString& url);
    void disconnect();
    void authenticate(const QString& sessionId);

    StreamingState state() const { return state_; }
    bool isConnected() const { return state_ == StreamingState::Connected || state_ == StreamingState::Authenticated; }
    bool isAuthenticated() const { return state_ == StreamingState::Authenticated; }

signals:
    void connected();
    void disconnected();
    void authenticated(qint64 userId, QList<qint64> accountIds);
    void authenticationFailed(const QString& reason);
    void accountSnapshotReceived(const AccountSnapshot& snapshot);
    void accountEventReceived(const QJsonObject& event);
    void orderFillReceived(const QJsonObject& fill);
    void errorOccurred(const QString& error);
    void stateChanged(StreamingState newState);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString& message);
    void onError(QAbstractSocket::SocketError error);

private:
    void setState(StreamingState newState);
    void handleMessage(const QJsonObject& msg);
    void handleAuthOk(const QJsonObject& msg);
    void handleAccountEvent(const QJsonObject& msg);
    void handleOrderFill(const QJsonObject& msg);
    void sendMessage(const QJsonObject& msg);

    QWebSocket webSocket_;
    StreamingState state_;
    QString sessionId_;
};
