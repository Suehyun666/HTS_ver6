#include "ViewModelFactory.h"
#include "../../viewmodel/order/OrderViewModel.h"
#include "../../viewmodel/chart/ChartViewModel.h"
#include "../../domain/service/order/OrderCommandService.h"
#include "../../domain/service/chart/ChartDataService.h"

ViewModelFactory& ViewModelFactory::instance() {
    static ViewModelFactory instance;
    return instance;
}

ViewModelFactory::ViewModelFactory(QObject* parent)
    : QObject(parent)
{
}

OrderViewModel* ViewModelFactory::createOrderViewModel(QObject* parent) {
    OrderCommandService* orderService = new OrderCommandService();
    return new OrderViewModel(orderService, parent);
}

ChartViewModel* ViewModelFactory::createChartViewModel(QObject* parent) {
    ChartDataService* chartService = new ChartDataService();
    return new ChartViewModel(chartService, parent);
}
