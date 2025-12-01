#include "ChartDataService.h"
#include "../../../infrastructure/database/OHLCVRepository.h"
#include <QDebug>

QVector<OHLCV> ChartDataService::getChartData(
    const QString& symbol,
    TimeInterval interval,
    int limit
) {
    // Ensure we have sample data
    ensureSampleDataExists(symbol, interval);

    // Get data from repository
    auto& repo = OHLCVRepository::instance();
    return repo.getLatestOHLCV(symbol, interval, limit);
}

bool ChartDataService::refreshData(const QString& symbol, TimeInterval interval) {
    auto& repo = OHLCVRepository::instance();

    repo.deleteOHLCV(symbol, interval);

    int count;
    switch (interval) {
        case TimeInterval::Second: count = 300; break;
        case TimeInterval::Minute: count = 300; break;
        case TimeInterval::Hour: count = 168; break;
        case TimeInterval::Day: count = 500; break;
        case TimeInterval::Month: count = 120; break;
        default: count = 500; break;
    }

    repo.generateRealisticData(symbol, interval, count);
    return true;
}

void ChartDataService::ensureSampleDataExists(const QString& symbol, TimeInterval interval) {
    auto& repo = OHLCVRepository::instance();

    if (!repo.hasData(symbol, TimeInterval::Day)) {
        qDebug() << "No data found for" << symbol << ", generating all intervals";
        repo.generateAllIntervalsFromDaily(symbol, 500);
    }
}
