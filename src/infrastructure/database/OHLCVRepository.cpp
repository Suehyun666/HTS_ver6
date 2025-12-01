#include "OHLCVRepository.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QRandomGenerator>

OHLCVRepository& OHLCVRepository::instance() {
    static OHLCVRepository instance;
    return instance;
}

OHLCVRepository::OHLCVRepository(QObject* parent)
    : QObject(parent)
{
}

OHLCVRepository::~OHLCVRepository() {
    close();
}

bool OHLCVRepository::initialize(const QString& dbPath) {
    QString path = dbPath;
    if (path.isEmpty()) {
        QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(dataDir);
        path = dataDir + "/ohlcv.db";
    }

    db_ = QSqlDatabase::addDatabase("QSQLITE", "ohlcv_connection");
    db_.setDatabaseName(path);

    if (!db_.open()) {
        qWarning() << "Failed to open OHLCV database:" << db_.lastError().text();
        return false;
    }

    return createTables();
}

void OHLCVRepository::close() {
    if (db_.isOpen()) {
        db_.close();
    }
}

bool OHLCVRepository::createTables() {
    QSqlQuery query(db_);

    QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS ohlcv (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            symbol TEXT NOT NULL,
            timestamp INTEGER NOT NULL,
            interval TEXT NOT NULL,
            open REAL NOT NULL,
            high REAL NOT NULL,
            low REAL NOT NULL,
            close REAL NOT NULL,
            volume INTEGER NOT NULL,
            UNIQUE(symbol, timestamp, interval)
        )
    )";

    if (!query.exec(createTableSQL)) {
        qWarning() << "Failed to create ohlcv table:" << query.lastError().text();
        return false;
    }

    query.exec("CREATE INDEX IF NOT EXISTS idx_symbol_interval_time ON ohlcv(symbol, interval, timestamp DESC)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_timestamp ON ohlcv(timestamp DESC)");

    return true;
}

QString OHLCVRepository::intervalToString(TimeInterval interval) const {
    switch (interval) {
        case TimeInterval::Second: return "1s";
        case TimeInterval::Minute: return "1m";
        case TimeInterval::Hour: return "1h";
        case TimeInterval::Day: return "1d";
        case TimeInterval::Month: return "1M";
        default: return "1d";
    }
}

TimeInterval OHLCVRepository::stringToInterval(const QString& str) const {
    if (str == "1s") return TimeInterval::Second;
    if (str == "1m") return TimeInterval::Minute;
    if (str == "1h") return TimeInterval::Hour;
    if (str == "1d") return TimeInterval::Day;
    if (str == "1M") return TimeInterval::Month;
    return TimeInterval::Day;
}

bool OHLCVRepository::insertOHLCV(const OHLCV& data) {
    QSqlQuery query(db_);
    query.prepare(R"(
        INSERT OR REPLACE INTO ohlcv
        (symbol, timestamp, interval, open, high, low, close, volume)
        VALUES (:symbol, :timestamp, :interval, :open, :high, :low, :close, :volume)
    )");

    query.bindValue(":symbol", data.symbol);
    query.bindValue(":timestamp", data.timestamp);
    query.bindValue(":interval", intervalToString(data.interval));
    query.bindValue(":open", data.open);
    query.bindValue(":high", data.high);
    query.bindValue(":low", data.low);
    query.bindValue(":close", data.close);
    query.bindValue(":volume", data.volume);

    if (!query.exec()) {
        qWarning() << "Failed to insert OHLCV:" << query.lastError().text();
        return false;
    }

    return true;
}

bool OHLCVRepository::insertBatch(const QVector<OHLCV>& dataList) {
    if (dataList.isEmpty()) {
        return true;
    }

    db_.transaction();

    QSqlQuery query(db_);
    query.prepare(R"(
        INSERT OR REPLACE INTO ohlcv
        (symbol, timestamp, interval, open, high, low, close, volume)
        VALUES (:symbol, :timestamp, :interval, :open, :high, :low, :close, :volume)
    )");

    QVariantList symbols, timestamps, intervals, opens, highs, lows, closes, volumes;
    symbols.reserve(dataList.size());
    timestamps.reserve(dataList.size());
    intervals.reserve(dataList.size());
    opens.reserve(dataList.size());
    highs.reserve(dataList.size());
    lows.reserve(dataList.size());
    closes.reserve(dataList.size());
    volumes.reserve(dataList.size());

    for (const auto& data : dataList) {
        symbols << data.symbol;
        timestamps << data.timestamp;
        intervals << intervalToString(data.interval);
        opens << data.open;
        highs << data.high;
        lows << data.low;
        closes << data.close;
        volumes << data.volume;
    }

    query.bindValue(":symbol", symbols);
    query.bindValue(":timestamp", timestamps);
    query.bindValue(":interval", intervals);
    query.bindValue(":open", opens);
    query.bindValue(":high", highs);
    query.bindValue(":low", lows);
    query.bindValue(":close", closes);
    query.bindValue(":volume", volumes);

    if (!query.execBatch()) {
        qWarning() << "Failed to batch insert OHLCV:" << query.lastError().text();
        db_.rollback();
        return false;
    }

    return db_.commit();
}

QVector<OHLCV> OHLCVRepository::getOHLCV(
    const QString& symbol,
    TimeInterval interval,
    qint64 startTime,
    qint64 endTime
) {
    QVector<OHLCV> result;
    QSqlQuery query(db_);

    query.prepare(R"(
        SELECT symbol, timestamp, interval, open, high, low, close, volume
        FROM ohlcv
        WHERE symbol = :symbol
          AND interval = :interval
          AND timestamp >= :startTime
          AND timestamp <= :endTime
        ORDER BY timestamp ASC
    )");

    query.bindValue(":symbol", symbol);
    query.bindValue(":interval", intervalToString(interval));
    query.bindValue(":startTime", startTime);
    query.bindValue(":endTime", endTime);

    if (!query.exec()) {
        qWarning() << "Failed to query OHLCV:" << query.lastError().text();
        return result;
    }

    while (query.next()) {
        OHLCV data;
        data.symbol = query.value(0).toString();
        data.timestamp = query.value(1).toLongLong();
        data.interval = stringToInterval(query.value(2).toString());
        data.open = query.value(3).toDouble();
        data.high = query.value(4).toDouble();
        data.low = query.value(5).toDouble();
        data.close = query.value(6).toDouble();
        data.volume = query.value(7).toLongLong();
        result.append(data);
    }

    return result;
}

QVector<OHLCV> OHLCVRepository::getLatestOHLCV(
    const QString& symbol,
    TimeInterval interval,
    int limit
) {
    QVector<OHLCV> result;
    QSqlQuery query(db_);

    query.prepare(R"(
        SELECT symbol, timestamp, interval, open, high, low, close, volume
        FROM ohlcv
        WHERE symbol = :symbol AND interval = :interval
        ORDER BY timestamp DESC
        LIMIT :limit
    )");

    query.bindValue(":symbol", symbol);
    query.bindValue(":interval", intervalToString(interval));
    query.bindValue(":limit", limit);

    if (!query.exec()) {
        qWarning() << "Failed to query latest OHLCV:" << query.lastError().text();
        return result;
    }

    while (query.next()) {
        OHLCV data;
        data.symbol = query.value(0).toString();
        data.timestamp = query.value(1).toLongLong();
        data.interval = stringToInterval(query.value(2).toString());
        data.open = query.value(3).toDouble();
        data.high = query.value(4).toDouble();
        data.low = query.value(5).toDouble();
        data.close = query.value(6).toDouble();
        data.volume = query.value(7).toLongLong();
        result.prepend(data);  // Reverse order (oldest first)
    }

    return result;
}

qint64 OHLCVRepository::getEarliestTimestamp(const QString& symbol, TimeInterval interval) {
    QSqlQuery query(db_);
    query.prepare(R"(
        SELECT MIN(timestamp) FROM ohlcv
        WHERE symbol = :symbol AND interval = :interval
    )");
    query.bindValue(":symbol", symbol);
    query.bindValue(":interval", intervalToString(interval));

    if (query.exec() && query.next()) {
        return query.value(0).toLongLong();
    }
    return 0;
}

qint64 OHLCVRepository::getLatestTimestamp(const QString& symbol, TimeInterval interval) {
    QSqlQuery query(db_);
    query.prepare(R"(
        SELECT MAX(timestamp) FROM ohlcv
        WHERE symbol = :symbol AND interval = :interval
    )");
    query.bindValue(":symbol", symbol);
    query.bindValue(":interval", intervalToString(interval));

    if (query.exec() && query.next()) {
        return query.value(0).toLongLong();
    }
    return 0;
}

int OHLCVRepository::getDataCount(const QString& symbol, TimeInterval interval) {
    QSqlQuery query(db_);
    query.prepare(R"(
        SELECT COUNT(*) FROM ohlcv
        WHERE symbol = :symbol AND interval = :interval
    )");
    query.bindValue(":symbol", symbol);
    query.bindValue(":interval", intervalToString(interval));

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

bool OHLCVRepository::hasData(const QString& symbol, TimeInterval interval) {
    return getDataCount(symbol, interval) > 0;
}

bool OHLCVRepository::deleteOHLCV(const QString& symbol, TimeInterval interval) {
    QSqlQuery query(db_);
    query.prepare("DELETE FROM ohlcv WHERE symbol = :symbol AND interval = :interval");
    query.bindValue(":symbol", symbol);
    query.bindValue(":interval", intervalToString(interval));

    if (!query.exec()) {
        qWarning() << "Failed to delete OHLCV:" << query.lastError().text();
        return false;
    }

    return true;
}

bool OHLCVRepository::deleteOldData(const QString& symbol, TimeInterval interval, qint64 beforeTimestamp) {
    QSqlQuery query(db_);
    query.prepare(R"(
        DELETE FROM ohlcv
        WHERE symbol = :symbol AND interval = :interval AND timestamp < :beforeTimestamp
    )");
    query.bindValue(":symbol", symbol);
    query.bindValue(":interval", intervalToString(interval));
    query.bindValue(":beforeTimestamp", beforeTimestamp);

    if (!query.exec()) {
        qWarning() << "Failed to delete old OHLCV data:" << query.lastError().text();
        return false;
    }

    return true;
}

bool OHLCVRepository::clearAll() {
    QSqlQuery query(db_);
    if (!query.exec("DELETE FROM ohlcv")) {
        qWarning() << "Failed to clear OHLCV table:" << query.lastError().text();
        return false;
    }
    return true;
}

void OHLCVRepository::generateSampleData(const QString& symbol, TimeInterval interval, int count) {
    QVector<OHLCV> dataList;
    dataList.reserve(count);

    QDateTime start(QDate(2024, 1, 1), QTime(0, 0), Qt::UTC);
    qint64 startTime = start.toSecsSinceEpoch();

    int timeStep;
    switch (interval) {
        case TimeInterval::Second: timeStep = 1; break;
        case TimeInterval::Minute: timeStep = 60; break;
        case TimeInterval::Hour: timeStep = 3600; break;
        case TimeInterval::Day: timeStep = 3600 * 24; break;
        case TimeInterval::Month: timeStep = 3600 * 24 * 30; break;
        default: timeStep = 3600 * 24; break;
    }

    double basePrice = 100.0;
    for (int i = 0; i < count; ++i) {
        OHLCV data;
        data.symbol = symbol;
        data.timestamp = startTime + (i * timeStep);
        data.interval = interval;

        double change = (QRandomGenerator::global()->bounded(1000) / 1000.0 - 0.5) * 5.0;
        basePrice = qMax(1.0, basePrice + change);

        data.open = basePrice;
        data.high = basePrice + qAbs(QRandomGenerator::global()->bounded(1000) / 1000.0 * 3.0);
        data.low = basePrice - qAbs(QRandomGenerator::global()->bounded(1000) / 1000.0 * 3.0);
        data.close = data.low + (data.high - data.low) * (QRandomGenerator::global()->bounded(1000) / 1000.0);
        data.volume = QRandomGenerator::global()->bounded(1000000, 5000000);

        basePrice = data.close;
        dataList.append(data);
    }

    insertBatch(dataList);
    qDebug() << "Generated" << count << "sample OHLCV records for" << symbol << intervalToString(interval);
}

void OHLCVRepository::generateRealisticData(const QString& symbol, TimeInterval interval, int count) {
    QVector<OHLCV> dataList;
    dataList.reserve(count);

    QDateTime endTime = QDateTime::currentDateTime().toUTC();
    int timeStep;
    switch (interval) {
        case TimeInterval::Second: timeStep = 1; break;
        case TimeInterval::Minute: timeStep = 60; break;
        case TimeInterval::Hour: timeStep = 3600; break;
        case TimeInterval::Day: timeStep = 3600 * 24; break;
        case TimeInterval::Month: timeStep = 3600 * 24 * 30; break;
        default: timeStep = 3600 * 24; break;
    }

    qint64 currentTime = endTime.toSecsSinceEpoch();
    qint64 startTime = currentTime - (count * timeStep);

    double basePrice = 150.0;
    double trend = 0.0;
    int trendDuration = 0;

    for (int i = 0; i < count; ++i) {
        OHLCV data;
        data.symbol = symbol;
        data.timestamp = startTime + (i * timeStep);
        data.interval = interval;

        if (trendDuration <= 0) {
            trend = (QRandomGenerator::global()->bounded(1000) / 1000.0 - 0.5) * 0.3;
            trendDuration = QRandomGenerator::global()->bounded(10, 50);
        }
        trendDuration--;

        double volatility = 0.02 + (QRandomGenerator::global()->bounded(100) / 1000.0);
        double change = trend + (QRandomGenerator::global()->bounded(1000) / 1000.0 - 0.5) * volatility;
        basePrice = qMax(10.0, basePrice * (1.0 + change));

        data.open = basePrice;
        double spread = basePrice * volatility;
        data.high = data.open + qAbs(QRandomGenerator::global()->bounded(1000) / 1000.0) * spread;
        data.low = data.open - qAbs(QRandomGenerator::global()->bounded(1000) / 1000.0) * spread;
        data.close = data.low + (data.high - data.low) * QRandomGenerator::global()->bounded(1000) / 1000.0;

        qint64 baseVolume = 1000000;
        double volumeMultiplier = 0.5 + (QRandomGenerator::global()->bounded(1000) / 500.0);
        data.volume = static_cast<qint64>(baseVolume * volumeMultiplier);

        basePrice = data.close;
        dataList.append(data);
    }

    insertBatch(dataList);
    qDebug() << "Generated" << count << "realistic OHLCV records for" << symbol << intervalToString(interval);
}

void OHLCVRepository::generateAllIntervalsFromDaily(const QString& symbol, int dayCount) {
    qDebug() << "Generating all interval data for" << symbol;

    QVector<OHLCV> dailyData;
    dailyData.reserve(dayCount);

    QDateTime endTime = QDateTime::currentDateTime().toUTC();
    qint64 currentTime = endTime.toSecsSinceEpoch();
    qint64 startTime = currentTime - (dayCount * 3600 * 24);

    double basePrice = 150.0;
    double trend = 0.0;
    int trendDuration = 0;

    for (int i = 0; i < dayCount; ++i) {
        OHLCV data;
        data.symbol = symbol;
        data.timestamp = startTime + (i * 3600 * 24);
        data.interval = TimeInterval::Day;

        if (trendDuration <= 0) {
            trend = (QRandomGenerator::global()->bounded(1000) / 1000.0 - 0.5) * 0.3;
            trendDuration = QRandomGenerator::global()->bounded(10, 50);
        }
        trendDuration--;

        double volatility = 0.02 + (QRandomGenerator::global()->bounded(100) / 1000.0);
        double change = trend + (QRandomGenerator::global()->bounded(1000) / 1000.0 - 0.5) * volatility;
        basePrice = qMax(10.0, basePrice * (1.0 + change));

        data.open = basePrice;
        double spread = basePrice * volatility;
        data.high = data.open + qAbs(QRandomGenerator::global()->bounded(1000) / 1000.0) * spread;
        data.low = data.open - qAbs(QRandomGenerator::global()->bounded(1000) / 1000.0) * spread;
        data.close = data.low + (data.high - data.low) * QRandomGenerator::global()->bounded(1000) / 1000.0;

        qint64 baseVolume = 5000000;
        double volumeMultiplier = 0.5 + (QRandomGenerator::global()->bounded(1000) / 500.0);
        data.volume = static_cast<qint64>(baseVolume * volumeMultiplier);

        basePrice = data.close;
        dailyData.append(data);
    }

    insertBatch(dailyData);
    qDebug() << "Generated" << dayCount << "daily candles for" << symbol;

    QVector<OHLCV> hourlyData;
    for (const auto& day : dailyData) {
        double priceRange = day.high - day.low;
        double currentPrice = day.open;

        for (int h = 0; h < 24; ++h) {
            OHLCV hour;
            hour.symbol = symbol;
            hour.timestamp = day.timestamp + (h * 3600);
            hour.interval = TimeInterval::Hour;

            hour.open = currentPrice;
            double hourMove = (QRandomGenerator::global()->bounded(1000) / 1000.0 - 0.5) * priceRange * 0.15;
            currentPrice += hourMove;
            currentPrice = qBound(day.low, currentPrice, day.high);

            hour.high = currentPrice + qAbs(QRandomGenerator::global()->bounded(1000) / 10000.0) * priceRange;
            hour.low = currentPrice - qAbs(QRandomGenerator::global()->bounded(1000) / 10000.0) * priceRange;
            hour.high = qMin(hour.high, day.high);
            hour.low = qMax(hour.low, day.low);
            hour.close = hour.low + (hour.high - hour.low) * (QRandomGenerator::global()->bounded(1000) / 1000.0);

            hour.volume = day.volume / 24 + QRandomGenerator::global()->bounded(-day.volume / 100, day.volume / 100);
            currentPrice = hour.close;

            hourlyData.append(hour);
        }
    }

    insertBatch(hourlyData);
    qDebug() << "Generated" << hourlyData.size() << "hourly candles for" << symbol;

    QVector<OHLCV> minuteData;
    for (int i = 0; i < hourlyData.size(); ++i) {
        const auto& hour = hourlyData[i];
        double priceRange = hour.high - hour.low;
        double currentPrice = hour.open;

        for (int m = 0; m < 60; ++m) {
            OHLCV minute;
            minute.symbol = symbol;
            minute.timestamp = hour.timestamp + (m * 60);
            minute.interval = TimeInterval::Minute;

            minute.open = currentPrice;
            double minuteMove = (QRandomGenerator::global()->bounded(1000) / 1000.0 - 0.5) * priceRange * 0.05;
            currentPrice += minuteMove;
            currentPrice = qBound(hour.low, currentPrice, hour.high);

            minute.high = currentPrice + qAbs(QRandomGenerator::global()->bounded(1000) / 100000.0) * priceRange;
            minute.low = currentPrice - qAbs(QRandomGenerator::global()->bounded(1000) / 100000.0) * priceRange;
            minute.high = qMin(minute.high, hour.high);
            minute.low = qMax(minute.low, hour.low);
            minute.close = minute.low + (minute.high - minute.low) * (QRandomGenerator::global()->bounded(1000) / 1000.0);

            minute.volume = hour.volume / 60 + QRandomGenerator::global()->bounded(-hour.volume / 200, hour.volume / 200);
            currentPrice = minute.close;

            minuteData.append(minute);
        }
    }

    insertBatch(minuteData);
    qDebug() << "Generated" << minuteData.size() << "minute candles for" << symbol;

    QVector<OHLCV> secondData;
    int minuteSampleCount = qMin(300, minuteData.size());
    int startIdx = qMax(0, minuteData.size() - minuteSampleCount);
    for (int i = startIdx; i < minuteData.size(); ++i) {
        const auto& minute = minuteData[i];
        double priceRange = minute.high - minute.low;
        double currentPrice = minute.open;

        for (int s = 0; s < 60; ++s) {
            OHLCV second;
            second.symbol = symbol;
            second.timestamp = minute.timestamp + s;
            second.interval = TimeInterval::Second;

            second.open = currentPrice;
            double secondMove = (QRandomGenerator::global()->bounded(1000) / 1000.0 - 0.5) * priceRange * 0.02;
            currentPrice += secondMove;
            currentPrice = qBound(minute.low, currentPrice, minute.high);

            second.high = currentPrice + qAbs(QRandomGenerator::global()->bounded(1000) / 1000000.0) * priceRange;
            second.low = currentPrice - qAbs(QRandomGenerator::global()->bounded(1000) / 1000000.0) * priceRange;
            second.high = qMin(second.high, minute.high);
            second.low = qMax(second.low, minute.low);
            second.close = second.low + (second.high - second.low) * (QRandomGenerator::global()->bounded(1000) / 1000.0);

            second.volume = minute.volume / 60 + QRandomGenerator::global()->bounded(-minute.volume / 200, minute.volume / 200);
            currentPrice = second.close;

            secondData.append(second);
        }
    }

    insertBatch(secondData);
    qDebug() << "Generated" << secondData.size() << "second candles for" << symbol;
    qDebug() << "Completed generating all intervals for" << symbol;
}
