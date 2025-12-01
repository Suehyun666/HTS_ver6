#pragma once

#include <QObject>
#include <QMap>
#include <QJsonObject>
#include "AccountViewModel.h"
#include "../../infrastructure/streaming/StreamingClient.h"

class AccountManager : public QObject {
    Q_OBJECT

public:
    static AccountManager* instance();
    static void initialize(StreamingClient* streamingClient);
    static void destroy();

    AccountViewModel* getAccountViewModel(qint64 accountId) const;
    QList<AccountViewModel*> getAllAccountViewModels() const;
    qint64 getPrimaryAccountId() const;

private slots:
    void onAccountSnapshotReceived(const AccountSnapshot& snapshot);
    void onAccountEventReceived(const QJsonObject& event);

private:
    explicit AccountManager(StreamingClient* streamingClient, QObject* parent = nullptr);
    ~AccountManager();

    AccountViewModel* getOrCreateViewModel(qint64 accountId);
    void handleReservedEvent(qint64 accountId, const QJsonObject& event);
    void handleReleasedEvent(qint64 accountId, const QJsonObject& event);
    void handleFilledEvent(qint64 accountId, const QJsonObject& event);

    static AccountManager* instance_;
    StreamingClient* streamingClient_;
    QMap<qint64, AccountViewModel*> viewModels_;
    qint64 primaryAccountId_;
};
