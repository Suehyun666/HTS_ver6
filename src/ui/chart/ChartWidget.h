#ifndef HTS_VER6_CHARTWIDGET_H
#define HTS_VER6_CHARTWIDGET_H
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QDate>
#include "../widget/DomainWidget.h"
#include "../plot/qcustomplot.h"

class QLineEdit;
class QPushButton;

class ChartWidget : public DomainWidget {
    Q_OBJECT
public:
    explicit ChartWidget(QWidget* parent = nullptr);

    QString windowId() const override { return "chart"; }
    QString windowTitle() const override { return tr("Chart"); }

protected:
    QSize preferredSize() const override { return QSize(900, 600); }
    QSize minimumSizeHint() const override { return QSize(600, 400); }
    QSize maximumSizeHint() const override { return QSize(1600, 1200); }

private slots:
    void onSymbolChanged();
    void onRefreshClicked();

private:
    void setupUI();
    void setupChart();
    void generateRandomData();

    QCustomPlot* customPlot_;
    QLineEdit* symbolEdit_;
    QString symbol_;
};

#endif
