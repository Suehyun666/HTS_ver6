#pragma once
#include <QString>
#include <QDateTime>

struct Session {
    QString sessionId;
    QString userId;
    QString userName;
    QDateTime loginTime;
    QDateTime expireTime;

    bool isValid() const {
        return !sessionId.isEmpty() && QDateTime::currentDateTime() < expireTime;
    }
};
