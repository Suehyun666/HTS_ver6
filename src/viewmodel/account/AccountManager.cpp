#include "AccountManager.h"
#include <QDebug>

AccountManager* AccountManager::instance_ = nullptr;

AccountManager* AccountManager::instance() {
    return instance_;
}

void AccountManager::initialize(StreamingClient* streamingClient) {
    if (!instance_) {
        instance_ = new AccountManager(streamingClient, nullptr);
    }
}

void AccountManager::destroy() {
    if (instance_) {
        delete instance_;
        instance_ = nullptr;
    }
}

AccountManager::AccountManager(StreamingClient* streamingClient, QObject* parent)
    : QObject(parent)
    , streamingClient_(streamingClient)
    , primaryAccountId_(0)
{
    connect(streamingClient_, &StreamingClient::accountSnapshotReceived,
            this, &AccountManager::onAccountSnapshotReceived);
    connect(streamingClient_, &StreamingClient::accountEventReceived,
            this, &AccountManager::onAccountEventReceived);
    connect(streamingClient_, &StreamingClient::authenticated,
            this, [this](qint64 userId, QList<qint64> accountIds) {
        if (!accountIds.isEmpty()) {
            primaryAccountId_ = accountIds.first();
            qDebug() << "[AccountManager] Primary account set to:" << primaryAccountId_;
        }
    });
}

AccountManager::~AccountManager() {
    qDeleteAll(viewModels_);
}

AccountViewModel* AccountManager::getAccountViewModel(qint64 accountId) const {
    return viewModels_.value(accountId, nullptr);
}

QList<AccountViewModel*> AccountManager::getAllAccountViewModels() const {
    return viewModels_.values();
}

qint64 AccountManager::getPrimaryAccountId() const {
    return primaryAccountId_;
}

AccountViewModel* AccountManager::getOrCreateViewModel(qint64 accountId) {
    AccountViewModel* viewModel = viewModels_.value(accountId, nullptr);
    if (!viewModel) {
        viewModel = new AccountViewModel(accountId, this);
        viewModels_.insert(accountId, viewModel);
        qDebug() << "[AccountManager] Created ViewModel for account" << accountId;
    }
    return viewModel;
}

void AccountManager::onAccountSnapshotReceived(const AccountSnapshot& snapshot) {
    qDebug() << "[AccountManager] Snapshot received:"
             << "accountId=" << snapshot.accountId
             << "balance=" << snapshot.cashBalance
             << "reserved=" << snapshot.cashReserved;

    AccountViewModel* viewModel = getOrCreateViewModel(snapshot.accountId);
    viewModel->updateSnapshot(snapshot.cashBalance, snapshot.cashReserved);
}

void AccountManager::onAccountEventReceived(const QJsonObject& event) {
    QString eventType = event["eventType"].toString();
    qint64 accountId = event["accountId"].toInteger();

    qDebug() << "[AccountManager] Event received:"
             << "type=" << eventType
             << "accountId=" << accountId;

    if (eventType == "RESERVED") {
        handleReservedEvent(accountId, event);
    } else if (eventType == "RELEASED") {
        handleReleasedEvent(accountId, event);
    } else if (eventType == "FILLED") {
        handleFilledEvent(accountId, event);
    } else {
        qWarning() << "[AccountManager] Unknown event type:" << eventType;
    }
}

void AccountManager::handleReservedEvent(qint64 accountId, const QJsonObject& event) {
    AccountViewModel* viewModel = getAccountViewModel(accountId);
    if (!viewModel) {
        qWarning() << "[AccountManager] No ViewModel for account" << accountId;
        return;
    }

    double amount = event["amountMicroUnits"].toInteger() / 1000000.0;
    double newReserved = viewModel->cashReserved() + amount;
    viewModel->updateReserved(newReserved);
}

void AccountManager::handleReleasedEvent(qint64 accountId, const QJsonObject& event) {
    AccountViewModel* viewModel = getAccountViewModel(accountId);
    if (!viewModel) {
        qWarning() << "[AccountManager] No ViewModel for account" << accountId;
        return;
    }

    double amount = event["amountMicroUnits"].toInteger() / 1000000.0;
    double newReserved = viewModel->cashReserved() - amount;
    viewModel->updateReserved(newReserved);
}

void AccountManager::handleFilledEvent(qint64 accountId, const QJsonObject& event) {
    AccountViewModel* viewModel = getAccountViewModel(accountId);
    if (!viewModel) {
        qWarning() << "[AccountManager] No ViewModel for account" << accountId;
        return;
    }

    double amount = event["amountMicroUnits"].toInteger() / 1000000.0;
    QString side = event["side"].toString();

    if (side == "BUY") {
        // Buy: decrease balance and reserved
        double newBalance = viewModel->cashBalance() - amount;
        double newReserved = viewModel->cashReserved() - amount;
        viewModel->updateSnapshot(newBalance, newReserved);
    } else if (side == "SELL") {
        // Sell: increase balance, decrease reserved
        double newBalance = viewModel->cashBalance() + amount;
        double newReserved = viewModel->cashReserved() - amount;
        viewModel->updateSnapshot(newBalance, newReserved);
    }
}
