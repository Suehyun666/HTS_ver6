//
// Created by suehyun on 11/22/25.
//

#ifndef HTS_VER6_DIGITALCLOCK_H
#define HTS_VER6_DIGITALCLOCK_H
#include <QLCDNumber>
#include <QTimer>
#include <QTime>
class DigitalClock : public QLCDNumber{
    Q_OBJECT

public:
    DigitalClock(QWidget *parent = nullptr);

private slots:
    void showTime();
};

#endif //HTS_VER6_DIGITALCLOCK_H