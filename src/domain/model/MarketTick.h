#pragma once
#include <QString>
#include <QDateTime>

struct MarketTick {
    QString symbol;
    double price;
    long volume;
    QDateTime timestamp;

    MarketTick()
        : price(0.0)
        , volume(0)
    {}
};
