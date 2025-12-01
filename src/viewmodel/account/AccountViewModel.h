#pragma once

#include <QObject>
#include <QString>

class AccountViewModel : public QObject {
    Q_OBJECT

public:
    explicit AccountViewModel(qint64 accountId, QObject* parent = nullptr);

    qint64 accountId() const { return accountId_; }
    double cashBalance() const { return cashBalance_; }
    double cashReserved() const { return cashReserved_; }
    double cashAvailable() const { return cashBalance_ - cashReserved_; }

    void updateBalance(double balance);
    void updateReserved(double reserved);
    void updateSnapshot(double balance, double reserved);

signals:
    void balanceChanged(double newBalance);
    void reservedChanged(double newReserved);
    void availableChanged(double newAvailable);
    void snapshotUpdated();

private:
    qint64 accountId_;
    double cashBalance_;
    double cashReserved_;
};
