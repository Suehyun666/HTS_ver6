#include "AccountViewModel.h"
#include <QDebug>

AccountViewModel::AccountViewModel(qint64 accountId, QObject* parent)
    : QObject(parent)
    , accountId_(accountId)
    , cashBalance_(0.0)
    , cashReserved_(0.0)
{
}

void AccountViewModel::updateBalance(double balance) {
    if (qAbs(cashBalance_ - balance) > 0.0001) {
        cashBalance_ = balance;
        emit balanceChanged(cashBalance_);
        emit availableChanged(cashAvailable());
        qDebug() << "[AccountViewModel] Balance updated:" << accountId_ << "balance=" << cashBalance_;
    }
}

void AccountViewModel::updateReserved(double reserved) {
    if (qAbs(cashReserved_ - reserved) > 0.0001) {
        cashReserved_ = reserved;
        emit reservedChanged(cashReserved_);
        emit availableChanged(cashAvailable());
        qDebug() << "[AccountViewModel] Reserved updated:" << accountId_ << "reserved=" << cashReserved_;
    }
}

void AccountViewModel::updateSnapshot(double balance, double reserved) {
    bool changed = false;

    if (qAbs(cashBalance_ - balance) > 0.0001) {
        cashBalance_ = balance;
        changed = true;
    }

    if (qAbs(cashReserved_ - reserved) > 0.0001) {
        cashReserved_ = reserved;
        changed = true;
    }

    if (changed) {
        emit balanceChanged(cashBalance_);
        emit reservedChanged(cashReserved_);
        emit availableChanged(cashAvailable());
        emit snapshotUpdated();
        qDebug() << "[AccountViewModel] Snapshot updated:" << accountId_
                 << "balance=" << cashBalance_ << "reserved=" << cashReserved_
                 << "available=" << cashAvailable();
    }
}
