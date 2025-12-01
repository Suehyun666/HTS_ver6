#pragma once
#include <QObject>

class OrderViewModel;
class ChartViewModel;

class ViewModelFactory : public QObject {
    Q_OBJECT

public:
    static ViewModelFactory& instance();

    OrderViewModel* createOrderViewModel(QObject* parent = nullptr);
    ChartViewModel* createChartViewModel(QObject* parent = nullptr);

private:
    explicit ViewModelFactory(QObject* parent = nullptr);
};
