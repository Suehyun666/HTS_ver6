#pragma once
#include <QString>

struct AccountSnapshot {
    QString accountId;
    double totalAssets;
    double cashBalance;
    double stockValuation;
    double purchasePower;

    AccountSnapshot()
        : totalAssets(0.0)
        , cashBalance(0.0)
        , stockValuation(0.0)
        , purchasePower(0.0)
    {}
};
