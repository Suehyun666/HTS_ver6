#ifndef CALCULATORWIDGET_H
#define CALCULATORWIDGET_H

#include "../../widget/DomainWidget.h"

class StockCalculator;

class CalculatorWidget : public DomainWidget {
    Q_OBJECT
public:
    explicit CalculatorWidget(QWidget* parent = nullptr);

    QString windowId() const override { return "calculator"; }
    QString windowTitle() const override { return tr("Calculator"); }
    bool isSingleton() const override { return true; }

protected:
    QSize preferredSize() const override { return QSize(320, 450); }
    QSize minimumSizeHint() const override { return QSize(280, 400); }
    QSize maximumSizeHint() const override { return QSize(400, 600); }

private:
    StockCalculator* calculator;
};

#endif
