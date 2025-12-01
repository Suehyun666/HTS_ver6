#pragma once
#include "../../model/OHLCV.h"
#include <QVector>
#include <QString>

class IChartDataService {
public:
    virtual ~IChartDataService() = default;

    // Get OHLCV data for chart display
    virtual QVector<OHLCV> getChartData(
        const QString& symbol,
        TimeInterval interval,
        int limit = 500
    ) = 0;

    // Refresh data from server (future implementation)
    virtual bool refreshData(const QString& symbol, TimeInterval interval) = 0;
};
