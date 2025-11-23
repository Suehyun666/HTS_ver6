#include "ChartWidget.h"
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QRandomGenerator>
#include <QDateTime>
#include <QTime>

ChartWidget::ChartWidget(QWidget* parent)
    : DomainWidget(parent)
    , customPlot_(nullptr)
    , symbolEdit_(nullptr)
    , symbol_("AAPL")
{
    setupUI();
    setupChart();
    generateRandomData();
}

void ChartWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    QHBoxLayout* topLayout = new QHBoxLayout();
    QLabel* symbolLabel = new QLabel(tr("Symbol:"));
    symbolEdit_ = new QLineEdit(symbol_);
    symbolEdit_->setMaximumWidth(100);
    QPushButton* refreshBtn = new QPushButton(tr("Refresh"));
    refreshBtn->setMaximumWidth(80);

    topLayout->addWidget(symbolLabel);
    topLayout->addWidget(symbolEdit_);
    topLayout->addWidget(refreshBtn);
    topLayout->addStretch();

    mainLayout->addLayout(topLayout);

    customPlot_ = new QCustomPlot(this);
    mainLayout->addWidget(customPlot_);

    connect(symbolEdit_, &QLineEdit::returnPressed, this, &ChartWidget::onSymbolChanged);
    connect(refreshBtn, &QPushButton::clicked, this, &ChartWidget::onRefreshClicked);
}

void ChartWidget::setupChart() {
    customPlot_->legend->setVisible(true);
    customPlot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    QCPFinancial *candlesticks = new QCPFinancial(customPlot_->xAxis, customPlot_->yAxis);
    candlesticks->setName("Candlestick");
    candlesticks->setChartStyle(QCPFinancial::csCandlestick);
    candlesticks->setTwoColored(true);
    candlesticks->setBrushPositive(QColor(245, 245, 245));
    candlesticks->setBrushNegative(QColor(40, 40, 40));
    candlesticks->setPenPositive(QPen(QColor(0, 0, 0)));
    candlesticks->setPenNegative(QPen(QColor(0, 0, 0)));

    QCPFinancial *ohlc = new QCPFinancial(customPlot_->xAxis, customPlot_->yAxis);
    ohlc->setName("OHLC");
    ohlc->setChartStyle(QCPFinancial::csOhlc);
    ohlc->setTwoColored(true);

    QCPAxisRect *volumeAxisRect = new QCPAxisRect(customPlot_);
    customPlot_->plotLayout()->addElement(1, 0, volumeAxisRect);
    volumeAxisRect->setMaximumSize(QSize(QWIDGETSIZE_MAX, 100));
    volumeAxisRect->axis(QCPAxis::atBottom)->setLayer("axes");
    volumeAxisRect->axis(QCPAxis::atBottom)->grid()->setLayer("grid");
    customPlot_->plotLayout()->setRowSpacing(0);
    volumeAxisRect->setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msBottom);
    volumeAxisRect->setMargins(QMargins(0, 0, 0, 0));

    customPlot_->setAutoAddPlottableToLegend(false);
    QCPBars *volumePos = new QCPBars(volumeAxisRect->axis(QCPAxis::atBottom), volumeAxisRect->axis(QCPAxis::atLeft));
    QCPBars *volumeNeg = new QCPBars(volumeAxisRect->axis(QCPAxis::atBottom), volumeAxisRect->axis(QCPAxis::atLeft));
    volumePos->setWidth(3600*4);
    volumePos->setPen(Qt::NoPen);
    volumePos->setBrush(QColor(100, 180, 110));
    volumeNeg->setWidth(3600*4);
    volumeNeg->setPen(Qt::NoPen);
    volumeNeg->setBrush(QColor(180, 90, 90));

    connect(customPlot_->xAxis, SIGNAL(rangeChanged(QCPRange)), volumeAxisRect->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(volumeAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), customPlot_->xAxis, SLOT(setRange(QCPRange)));

    QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
    dateTimeTicker->setDateTimeSpec(Qt::UTC);
    dateTimeTicker->setDateTimeFormat("dd. MMMM");
    volumeAxisRect->axis(QCPAxis::atBottom)->setTicker(dateTimeTicker);
    volumeAxisRect->axis(QCPAxis::atBottom)->setTickLabelRotation(15);
    customPlot_->xAxis->setBasePen(Qt::NoPen);
    customPlot_->xAxis->setTickLabels(false);
    customPlot_->xAxis->setTicks(false);
    customPlot_->xAxis->setTicker(dateTimeTicker);

    QCPMarginGroup *group = new QCPMarginGroup(customPlot_);
    customPlot_->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    volumeAxisRect->setMarginGroup(QCP::msLeft|QCP::msRight, group);
}

void ChartWidget::generateRandomData() {
    int n = 500;
    QVector<double> time(n), value1(n), value2(n);
    QDateTime start(QDate(2024, 11, 1), QTime(0, 0), Qt::UTC);
    double startTime = start.toSecsSinceEpoch();
    double binSize = 3600*24;

    time[0] = startTime;
    value1[0] = 60;
    value2[0] = 20;

    for (int i=1; i<n; ++i) {
        time[i] = startTime + 3600*i;
        value1[i] = value1[i-1] + (QRandomGenerator::global()->bounded(1000) / 1000.0 - 0.5) * 10;
        value2[i] = value2[i-1] + (QRandomGenerator::global()->bounded(1000) / 1000.0 - 0.5) * 3;
    }

    QCPFinancial *candlesticks = qobject_cast<QCPFinancial*>(customPlot_->plottable(0));
    QCPFinancial *ohlc = qobject_cast<QCPFinancial*>(customPlot_->plottable(1));

    if (candlesticks) {
        candlesticks->data()->set(QCPFinancial::timeSeriesToOhlc(time, value1, binSize, startTime));
        candlesticks->setWidth(binSize*0.9);
    }

    if (ohlc) {
        ohlc->data()->set(QCPFinancial::timeSeriesToOhlc(time, value2, binSize/3.0, startTime));
        ohlc->setWidth(binSize*0.2);
    }

    QCPAxisRect *volumeAxisRect = customPlot_->axisRect(1);
    QCPBars *volumePos = qobject_cast<QCPBars*>(customPlot_->plottable(2));
    QCPBars *volumeNeg = qobject_cast<QCPBars*>(customPlot_->plottable(3));

    if (volumePos && volumeNeg) {
        for (int i=0; i<n/5; ++i) {
            int v = QRandomGenerator::global()->bounded(20000) +
                    QRandomGenerator::global()->bounded(20000) +
                    QRandomGenerator::global()->bounded(20000) - 10000*3;
            (v < 0 ? volumeNeg : volumePos)->addData(startTime + 3600*5.0*i, qAbs(v));
        }
    }

    customPlot_->rescaleAxes();
    customPlot_->xAxis->scaleRange(1.025, customPlot_->xAxis->range().center());
    customPlot_->yAxis->scaleRange(1.1, customPlot_->yAxis->range().center());
    customPlot_->replot();
}

void ChartWidget::onSymbolChanged() {
    symbol_ = symbolEdit_->text().trimmed().toUpper();
    if (!symbol_.isEmpty()) {
        generateRandomData();
    }
}

void ChartWidget::onRefreshClicked() {
    generateRandomData();
}
