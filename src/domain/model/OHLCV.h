#pragma once
#include <QString>
#include <QDateTime>

enum class TimeInterval {
    Second,    // 1초
    Minute,    // 1분
    Hour,      // 1시간
    Day,       // 1일
    Month      // 1달
};

struct OHLCV {
    QString symbol;          // 심볼 (예: AAPL)
    qint64 timestamp;        // Unix timestamp (seconds)
    TimeInterval interval;   // 시간 간격
    double open;             // 시가
    double high;             // 고가
    double low;              // 저가
    double close;            // 종가
    qint64 volume;           // 거래량

    QDateTime dateTime() const {
        return QDateTime::fromSecsSinceEpoch(timestamp, Qt::UTC);
    }
};
