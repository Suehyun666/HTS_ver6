#ifndef STOCKCALCULATOR_H
#define STOCKCALCULATOR_H

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>

class StockCalculator : public QWidget {
    Q_OBJECT
public:
    explicit StockCalculator(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void calculate();
    void clearDisplay();
    void backspace();
    void appendToExpression(const QString& text);

private:
    QTextEdit* expressionDisplay;
    QLineEdit* resultDisplay;
    QString currentExpression;

    void setupUi();
    void setupConnections();
    double evaluateExpression(const QString& expr);
};

#endif
