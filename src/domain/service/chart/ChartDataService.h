#pragma once
#include "IChartDataService.h"

class ChartDataService : public IChartDataService {
public:
    ChartDataService() = default;

    QVector<OHLCV> getChartData(
        const QString& symbol,
        TimeInterval interval,
        int limit = 500
    ) override;

    bool refreshData(const QString& symbol, TimeInterval interval) override;

private:
    void ensureSampleDataExists(const QString& symbol, TimeInterval interval);
};
