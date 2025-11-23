#include "CalculatorWidget.h"
#include "StockCalculator.h"
#include <QVBoxLayout>

CalculatorWidget::CalculatorWidget(QWidget* parent) : DomainWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    calculator = new StockCalculator(this);
    layout->addWidget(calculator);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}
