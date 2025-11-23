#include "StockCalculator.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QKeyEvent>
#include <QJSEngine>
#include <cmath>

StockCalculator::StockCalculator(QWidget *parent) : QWidget(parent) {
    setupUi();
    setupConnections();
    setFocusPolicy(Qt::StrongFocus);
}

void StockCalculator::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);

    expressionDisplay = new QTextEdit();
    expressionDisplay->setMaximumHeight(60);
    expressionDisplay->setReadOnly(true);
    expressionDisplay->setStyleSheet("QTextEdit { font-size: 12pt; }");

    resultDisplay = new QLineEdit("0");
    resultDisplay->setReadOnly(true);
    resultDisplay->setAlignment(Qt::AlignRight);
    resultDisplay->setStyleSheet("QLineEdit { font-size: 16pt; font-weight: bold; }");

    mainLayout->addWidget(expressionDisplay);
    mainLayout->addWidget(resultDisplay);

    auto* buttonLayout = new QGridLayout();

    QStringList buttons = {
        "7", "8", "9", "/", "C",
        "4", "5", "6", "*", "←",
        "1", "2", "3", "-", "%",
        "0", ".", "=", "+", ""
    };

    int row = 0, col = 0;
    for (const QString& btnText : buttons) {
        if (btnText.isEmpty()) {
            col++;
            continue;
        }

        QPushButton* btn = new QPushButton(btnText);
        btn->setMinimumSize(50, 40);

        if (btnText == "=") {
            btn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; }");
            connect(btn, &QPushButton::clicked, this, &StockCalculator::calculate);
        } else if (btnText == "C") {
            btn->setStyleSheet("QPushButton { background-color: #f44336; color: white; }");
            connect(btn, &QPushButton::clicked, this, &StockCalculator::clearDisplay);
        } else if (btnText == "←") {
            connect(btn, &QPushButton::clicked, this, &StockCalculator::backspace);
        } else {
            connect(btn, &QPushButton::clicked, this, [this, btnText]() {
                appendToExpression(btnText);
            });
        }

        buttonLayout->addWidget(btn, row, col);

        col++;
        if (col >= 5) {
            col = 0;
            row++;
        }
    }

    mainLayout->addLayout(buttonLayout);
}

void StockCalculator::setupConnections() {
}

void StockCalculator::appendToExpression(const QString& text) {
    currentExpression += text;
    expressionDisplay->setText(currentExpression);
}

void StockCalculator::calculate() {
    if (currentExpression.isEmpty()) return;

    double result = evaluateExpression(currentExpression);
    resultDisplay->setText(QString::number(result, 'f', 2));
}

void StockCalculator::clearDisplay() {
    currentExpression.clear();
    expressionDisplay->clear();
    resultDisplay->setText("0");
}

void StockCalculator::backspace() {
    if (!currentExpression.isEmpty()) {
        currentExpression.chop(1);
        expressionDisplay->setText(currentExpression);
    }
}

double StockCalculator::evaluateExpression(const QString& expr) {
    QJSEngine engine;
    QJSValue result = engine.evaluate(expr);

    if (result.isNumber()) {
        return result.toNumber();
    }
    return 0.0;
}

void StockCalculator::keyPressEvent(QKeyEvent *event) {
    QString key = event->text();

    if (key >= "0" && key <= "9") {
        appendToExpression(key);
    } else if (key == "+" || key == "-" || key == "*" || key == "/" || key == "." || key == "%") {
        appendToExpression(key);
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        calculate();
    } else if (event->key() == Qt::Key_Backspace) {
        backspace();
    } else if (event->key() == Qt::Key_Escape || key.toLower() == "c") {
        clearDisplay();
    } else {
        QWidget::keyPressEvent(event);
    }
}
