#pragma once
#include <QString>
#include <QDateTime>

struct Session {
    QString sessionId;
    qint64 accountId;
    QDateTime loginTime;
    QDateTime expireTime;

    bool isValid() const {
        return !sessionId.isEmpty() && QDateTime::currentDateTime() < expireTime;
    }
};
