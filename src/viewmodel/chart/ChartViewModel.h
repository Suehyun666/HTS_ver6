#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include "../../domain/model/OHLCV.h"
#include "../../domain/service/chart/IChartDataService.h"

class ChartViewModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString symbol READ symbol WRITE setSymbol NOTIFY symbolChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    explicit ChartViewModel(IChartDataService* chartService, QObject* parent = nullptr);

    QString symbol() const { return symbol_; }
    bool loading() const { return loading_; }

    void setSymbol(const QString& symbol);

    Q_INVOKABLE void loadData(TimeInterval interval);
    Q_INVOKABLE void refreshData(TimeInterval interval);

signals:
    void symbolChanged();
    void loadingChanged();
    void dataLoaded(const QVector<OHLCV>& data);
    void dataLoadFailed(const QString& error);

private:
    void setLoading(bool loading);

    IChartDataService* chartService_;
    QString symbol_;
    bool loading_;
    TimeInterval currentInterval_;
};
