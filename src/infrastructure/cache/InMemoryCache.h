#pragma once
#include <QObject>
#include <QHash>
#include <QMutex>
#include <QDateTime>
#include <optional>

template<typename T>
struct CacheEntry {
    T value;
    QDateTime expireTime;

    bool isExpired() const {
        return QDateTime::currentDateTime() >= expireTime;
    }
};

template<typename T>
class InMemoryCache : public QObject {
public:
    explicit InMemoryCache(QObject* parent = nullptr)
        : QObject(parent) {}

    void put(const QString& key, const T& value, int ttlSeconds = 3600) {
        QMutexLocker locker(&mutex_);
        CacheEntry<T> entry;
        entry.value = value;
        entry.expireTime = QDateTime::currentDateTime().addSecs(ttlSeconds);
        cache_[key] = entry;
    }

    std::optional<T> get(const QString& key) {
        QMutexLocker locker(&mutex_);

        auto it = cache_.find(key);
        if (it == cache_.end()) {
            return std::nullopt;
        }

        if (it.value().isExpired()) {
            cache_.erase(it);
            return std::nullopt;
        }

        return it.value().value;
    }

    void remove(const QString& key) {
        QMutexLocker locker(&mutex_);
        cache_.remove(key);
    }

    void clear() {
        QMutexLocker locker(&mutex_);
        cache_.clear();
    }

    bool contains(const QString& key) const {
        QMutexLocker locker(&mutex_);
        auto it = cache_.find(key);
        if (it == cache_.end()) {
            return false;
        }
        return !it.value().isExpired();
    }

    int size() const {
        QMutexLocker locker(&mutex_);
        return cache_.size();
    }

    void cleanup() {
        QMutexLocker locker(&mutex_);
        auto it = cache_.begin();
        while (it != cache_.end()) {
            if (it.value().isExpired()) {
                it = cache_.erase(it);
            } else {
                ++it;
            }
        }
    }

private:
    QHash<QString, CacheEntry<T>> cache_;
    mutable QMutex mutex_;
};
