#pragma once
#include "../../domain/model/OHLCV.h"
#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QVector>

class OHLCVRepository : public QObject {
    Q_OBJECT

public:
    static OHLCVRepository& instance();

    bool initialize(const QString& dbPath = "");
    void close();

    // CRUD operations
    bool insertOHLCV(const OHLCV& data);
    bool insertBatch(const QVector<OHLCV>& dataList);

    QVector<OHLCV> getOHLCV(
        const QString& symbol,
        TimeInterval interval,
        qint64 startTime,
        qint64 endTime
    );

    QVector<OHLCV> getLatestOHLCV(
        const QString& symbol,
        TimeInterval interval,
        int limit = 500
    );

    qint64 getEarliestTimestamp(const QString& symbol, TimeInterval interval);
    qint64 getLatestTimestamp(const QString& symbol, TimeInterval interval);
    int getDataCount(const QString& symbol, TimeInterval interval);
    bool hasData(const QString& symbol, TimeInterval interval);

    bool deleteOHLCV(const QString& symbol, TimeInterval interval);
    bool deleteOldData(const QString& symbol, TimeInterval interval, qint64 beforeTimestamp);
    bool clearAll();

    void generateSampleData(const QString& symbol, TimeInterval interval, int count = 500);
    void generateRealisticData(const QString& symbol, TimeInterval interval, int count = 500);
    void generateAllIntervalsFromDaily(const QString& symbol, int dayCount = 500);

private:
    explicit OHLCVRepository(QObject* parent = nullptr);
    ~OHLCVRepository();

    bool createTables();
    QString intervalToString(TimeInterval interval) const;
    TimeInterval stringToInterval(const QString& str) const;

    QSqlDatabase db_;
};
