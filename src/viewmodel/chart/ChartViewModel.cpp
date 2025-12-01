#include "ChartViewModel.h"
#include <QDebug>

ChartViewModel::ChartViewModel(IChartDataService* chartService, QObject* parent)
    : QObject(parent)
    , chartService_(chartService)
    , symbol_("AAPL")
    , loading_(false)
    , currentInterval_(TimeInterval::Day)
{
}

void ChartViewModel::setSymbol(const QString& symbol) {
    if (symbol_ != symbol) {
        symbol_ = symbol;
        emit symbolChanged();
        // Auto-load data when symbol changes
        loadData(currentInterval_);
    }
}

void ChartViewModel::setLoading(bool loading) {
    if (loading_ != loading) {
        loading_ = loading;
        emit loadingChanged();
    }
}

void ChartViewModel::loadData(TimeInterval interval) {
    if (symbol_.isEmpty()) {
        emit dataLoadFailed("Symbol cannot be empty");
        return;
    }

    setLoading(true);
    currentInterval_ = interval;

    // Get data from service
    QVector<OHLCV> data = chartService_->getChartData(symbol_, interval, 500);

    setLoading(false);

    if (data.isEmpty()) {
        emit dataLoadFailed("No data available for " + symbol_);
    } else {
        emit dataLoaded(data);
    }
}

void ChartViewModel::refreshData(TimeInterval interval) {
    if (symbol_.isEmpty()) {
        emit dataLoadFailed("Symbol cannot be empty");
        return;
    }

    setLoading(true);
    currentInterval_ = interval;

    // Refresh from server/database
    bool success = chartService_->refreshData(symbol_, interval);

    if (success) {
        // Load the refreshed data
        QVector<OHLCV> data = chartService_->getChartData(symbol_, interval, 500);
        setLoading(false);

        if (!data.isEmpty()) {
            emit dataLoaded(data);
        } else {
            emit dataLoadFailed("Failed to load refreshed data");
        }
    } else {
        setLoading(false);
        emit dataLoadFailed("Failed to refresh data");
    }
}
