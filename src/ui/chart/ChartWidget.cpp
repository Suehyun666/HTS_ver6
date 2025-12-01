#include "ChartWidget.h"
#include "../../viewmodel/chart/ChartViewModel.h"
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QRandomGenerator>
#include <QDateTime>
#include <QTime>
#include <QMenu>
#include <QAction>
#include <QButtonGroup>
#include <QContextMenuEvent>
#include <QMessageBox>

ChartWidget::ChartWidget(ChartViewModel* viewModel, QWidget* parent)
    : DomainWidget(parent)
    , viewModel_(viewModel)
    , customPlot_(nullptr)
    , symbolEdit_(nullptr)
    , currentInterval_(TimeInterval::Day)
    , timeIntervalGroup_(nullptr)
    , drawingMenu_(nullptr)
    , dataPointCount_(0)
{
    setupUI();
    setupChart();
    connectViewModel();

    connect(customPlot_->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(onXRangeChanged(QCPRange)));
    connect(customPlot_->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(onYRangeChanged(QCPRange)));

    if (viewModel_) {
        viewModel_->loadData(currentInterval_);
    }
}

void ChartWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    // Top bar with symbol and time interval buttons
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->setSpacing(8);

    symbolEdit_ = new QLineEdit(viewModel_ ? viewModel_->symbol() : "AAPL");
    symbolEdit_->setMaximumWidth(120);
    symbolEdit_->setMinimumHeight(32);
    symbolEdit_->setPlaceholderText(tr("Symbol"));
    symbolEdit_->setStyleSheet("QLineEdit { font-size: 14px; font-weight: 500; }");

    QPushButton* refreshBtn = new QPushButton(tr("↻"));
    refreshBtn->setMaximumWidth(36);
    refreshBtn->setMinimumHeight(32);
    refreshBtn->setToolTip(tr("Refresh Chart"));
    refreshBtn->setStyleSheet("QPushButton { font-size: 18px; font-weight: bold; }");

    topLayout->addWidget(symbolEdit_);
    topLayout->addWidget(refreshBtn);
    topLayout->addSpacing(16);

    // Time interval buttons
    timeIntervalGroup_ = new QButtonGroup(this);
    QStringList intervals = {tr("1S"), tr("1M"), tr("1H"), tr("1D"), tr("1M")};  // 1초, 1분, 1시간, 1일, 1달
    QList<TimeInterval> intervalEnums = {
        TimeInterval::Second, TimeInterval::Minute, TimeInterval::Hour,
        TimeInterval::Day, TimeInterval::Month
    };

    QString buttonStyle =
        "QPushButton { "
        "  background-color: transparent; "
        "  border: 1px solid palette(mid); "
        "  border-radius: 3px; "
        "  padding: 4px 12px; "
        "  font-size: 12px; "
        "  min-width: 32px; "
        "  min-height: 28px; "
        "} "
        "QPushButton:checked { "
        "  background-color: #3b82f6; "
        "  color: white; "
        "  border-color: #3b82f6; "
        "} "
        "QPushButton:hover:!checked { "
        "  background-color: palette(light); "
        "}";

    for (int i = 0; i < intervals.size(); ++i) {
        QPushButton* btn = new QPushButton(intervals[i]);
        btn->setCheckable(true);
        btn->setStyleSheet(buttonStyle);
        timeIntervalGroup_->addButton(btn, static_cast<int>(intervalEnums[i]));
        topLayout->addWidget(btn);

        if (intervalEnums[i] == TimeInterval::Day) {
            btn->setChecked(true);  // Default to 1D
        }
    }

    topLayout->addStretch();

    mainLayout->addLayout(topLayout);

    // Chart takes most of the space
    customPlot_ = new QCustomPlot(this);
    customPlot_->setBackground(QBrush(Qt::transparent));
    customPlot_->setContextMenuPolicy(Qt::DefaultContextMenu);
    mainLayout->addWidget(customPlot_, 1);

    // Setup drawing menu (right-click menu)
    drawingMenu_ = new QMenu(this);
    drawingMenu_->addAction(tr("Horizontal Line"))->setEnabled(false);
    drawingMenu_->addAction(tr("Vertical Line"))->setEnabled(false);
    drawingMenu_->addAction(tr("Trend Line"))->setEnabled(false);
    drawingMenu_->addSeparator();
    drawingMenu_->addAction(tr("Rectangle"))->setEnabled(false);
    drawingMenu_->addAction(tr("Fibonacci Retracement"))->setEnabled(false);
    drawingMenu_->addSeparator();
    drawingMenu_->addAction(tr("Text"))->setEnabled(false);
    drawingMenu_->addAction(tr("Arrow"))->setEnabled(false);

    connect(symbolEdit_, &QLineEdit::returnPressed, this, &ChartWidget::onSymbolChanged);
    connect(refreshBtn, &QPushButton::clicked, this, &ChartWidget::onRefreshClicked);
    connect(timeIntervalGroup_, QOverload<int>::of(&QButtonGroup::idClicked),
            this, &ChartWidget::onTimeIntervalChanged);
}

void ChartWidget::setupChart() {
    customPlot_->legend->setVisible(false);
    customPlot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    customPlot_->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    customPlot_->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);

    // Modern candlestick chart - green/red
    QCPFinancial *candlesticks = new QCPFinancial(customPlot_->xAxis, customPlot_->yAxis);
    candlesticks->setName("Candlestick");
    candlesticks->setChartStyle(QCPFinancial::csCandlestick);
    candlesticks->setTwoColored(true);
    candlesticks->setBrushPositive(QColor(16, 185, 129));   // Green for up
    candlesticks->setBrushNegative(QColor(239, 68, 68));    // Red for down
    candlesticks->setPenPositive(QPen(QColor(16, 185, 129), 1));
    candlesticks->setPenNegative(QPen(QColor(239, 68, 68), 1));

    // OHLC overlay - thinner style
    QCPFinancial *ohlc = new QCPFinancial(customPlot_->xAxis, customPlot_->yAxis);
    ohlc->setName("OHLC");
    ohlc->setChartStyle(QCPFinancial::csOhlc);
    ohlc->setTwoColored(true);
    ohlc->setPenPositive(QPen(QColor(16, 185, 129, 150), 1));
    ohlc->setPenNegative(QPen(QColor(239, 68, 68, 150), 1));

    // Volume bars at bottom - smaller and subtle
    QCPAxisRect *volumeAxisRect = new QCPAxisRect(customPlot_);
    customPlot_->plotLayout()->addElement(1, 0, volumeAxisRect);
    volumeAxisRect->setMaximumSize(QSize(QWIDGETSIZE_MAX, 80));  // Smaller volume chart
    volumeAxisRect->axis(QCPAxis::atBottom)->setLayer("axes");
    volumeAxisRect->axis(QCPAxis::atBottom)->grid()->setLayer("grid");
    customPlot_->plotLayout()->setRowSpacing(5);
    volumeAxisRect->setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msBottom);
    volumeAxisRect->setMargins(QMargins(0, 0, 0, 0));

    customPlot_->setAutoAddPlottableToLegend(false);
    QCPBars *volumePos = new QCPBars(volumeAxisRect->axis(QCPAxis::atBottom), volumeAxisRect->axis(QCPAxis::atLeft));
    QCPBars *volumeNeg = new QCPBars(volumeAxisRect->axis(QCPAxis::atBottom), volumeAxisRect->axis(QCPAxis::atLeft));
    volumePos->setPen(Qt::NoPen);
    volumePos->setBrush(QColor(16, 185, 129, 100));
    volumeNeg->setPen(Qt::NoPen);
    volumeNeg->setBrush(QColor(239, 68, 68, 100));

    // Sync axes
    connect(customPlot_->xAxis, SIGNAL(rangeChanged(QCPRange)), volumeAxisRect->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(volumeAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), customPlot_->xAxis, SLOT(setRange(QCPRange)));

    // Date/time formatting - minimal
    QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
    dateTimeTicker->setDateTimeSpec(Qt::UTC);
    dateTimeTicker->setDateTimeFormat("MMM dd");  // Shorter format
    volumeAxisRect->axis(QCPAxis::atBottom)->setTicker(dateTimeTicker);
    volumeAxisRect->axis(QCPAxis::atBottom)->setTickLabelRotation(0);  // No rotation

    // Hide top axis labels for cleaner look
    customPlot_->xAxis->setBasePen(Qt::NoPen);
    customPlot_->xAxis->setTickLabels(false);
    customPlot_->xAxis->setTicks(false);
    customPlot_->xAxis->setTicker(dateTimeTicker);

    // Grid styling - subtle
    customPlot_->xAxis->grid()->setPen(QPen(QColor(150, 150, 150, 50), 1, Qt::DotLine));
    customPlot_->yAxis->grid()->setPen(QPen(QColor(150, 150, 150, 50), 1, Qt::DotLine));
    volumeAxisRect->axis(QCPAxis::atBottom)->grid()->setPen(QPen(QColor(150, 150, 150, 30), 1, Qt::DotLine));

    // Align margins
    QCPMarginGroup *group = new QCPMarginGroup(customPlot_);
    customPlot_->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    volumeAxisRect->setMarginGroup(QCP::msLeft|QCP::msRight, group);
}

void ChartWidget::displayData(const QVector<OHLCV>& data) {
    if (data.isEmpty()) {
        return;
    }

    // Calculate bin size based on interval
    double binSize;
    switch (currentInterval_) {
        case TimeInterval::Second: binSize = 1; break;
        case TimeInterval::Minute: binSize = 60; break;
        case TimeInterval::Hour: binSize = 3600; break;
        case TimeInterval::Day: binSize = 3600*24; break;
        case TimeInterval::Month: binSize = 3600*24*30; break;
        default: binSize = 3600*24; break;
    }

    // Get chart plottables
    QCPFinancial *candlesticks = qobject_cast<QCPFinancial*>(customPlot_->plottable(0));
    QCPFinancial *ohlc = qobject_cast<QCPFinancial*>(customPlot_->plottable(1));
    QCPBars *volumePos = qobject_cast<QCPBars*>(customPlot_->plottable(2));
    QCPBars *volumeNeg = qobject_cast<QCPBars*>(customPlot_->plottable(3));

    // Clear existing data
    if (candlesticks) candlesticks->data()->clear();
    if (ohlc) ohlc->data()->clear();
    if (volumePos) volumePos->data()->clear();
    if (volumeNeg) volumeNeg->data()->clear();

    // Convert OHLCV data to QCustomPlot format
    for (const auto& bar : data) {
        double time = static_cast<double>(bar.timestamp);

        // Add candlestick data
        if (candlesticks) {
            candlesticks->data()->add(QCPFinancialData(time, bar.open, bar.high, bar.low, bar.close));
        }

        // Add OHLC overlay (using same data but different visual style)
        if (ohlc) {
            ohlc->data()->add(QCPFinancialData(time, bar.open, bar.high, bar.low, bar.close));
        }

        // Add volume bars (positive if close > open, negative otherwise)
        bool isPositive = bar.close >= bar.open;
        if (isPositive && volumePos) {
            volumePos->addData(time, static_cast<double>(bar.volume));
        } else if (!isPositive && volumeNeg) {
            volumeNeg->addData(time, static_cast<double>(bar.volume));
        }
    }

    if (candlesticks) candlesticks->setWidth(binSize * 0.9);
    if (ohlc) ohlc->setWidth(binSize * 0.2);
    if (volumePos) volumePos->setWidth(binSize * 0.9);
    if (volumeNeg) volumeNeg->setWidth(binSize * 0.9);

    customPlot_->rescaleAxes();
    customPlot_->xAxis->scaleRange(1.05, customPlot_->xAxis->range().center());
    customPlot_->yAxis->scaleRange(1.1, customPlot_->yAxis->range().center());

    initialXRange_ = customPlot_->xAxis->range();
    initialYRange_ = customPlot_->yAxis->range();
    dataPointCount_ = data.size();

    updatePriceAxisTicks();
    customPlot_->replot();
}

void ChartWidget::onXRangeChanged(const QCPRange& newRange) {
    if (dataPointCount_ == 0) return;

    double binSize;
    switch (currentInterval_) {
        case TimeInterval::Second: binSize = 1; break;
        case TimeInterval::Minute: binSize = 60; break;
        case TimeInterval::Hour: binSize = 3600; break;
        case TimeInterval::Day: binSize = 3600*24; break;
        case TimeInterval::Month: binSize = 3600*24*30; break;
        default: binSize = 3600*24; break;
    }

    double minVisible = 10 * binSize;
    double maxVisible = initialXRange_.size() * 1.2;

    QCPRange bounded = newRange;
    if (bounded.size() < minVisible) {
        bounded = QCPRange(bounded.center() - minVisible/2, bounded.center() + minVisible/2);
    }
    if (bounded.size() > maxVisible) {
        bounded = QCPRange(bounded.center() - maxVisible/2, bounded.center() + maxVisible/2);
    }

    if (bounded != newRange) {
        customPlot_->xAxis->blockSignals(true);
        customPlot_->xAxis->setRange(bounded);
        customPlot_->xAxis->blockSignals(false);
    }
}

void ChartWidget::onYRangeChanged(const QCPRange& newRange) {
    if (dataPointCount_ == 0) return;

    double minVisible = initialYRange_.size() * 0.1;
    double maxVisible = initialYRange_.size() * 5.0;

    QCPRange bounded = newRange;
    if (bounded.size() < minVisible) {
        bounded = QCPRange(bounded.center() - minVisible/2, bounded.center() + minVisible/2);
    }
    if (bounded.size() > maxVisible) {
        bounded = QCPRange(bounded.center() - maxVisible/2, bounded.center() + maxVisible/2);
    }

    if (bounded != newRange) {
        customPlot_->yAxis->blockSignals(true);
        customPlot_->yAxis->setRange(bounded);
        customPlot_->yAxis->blockSignals(false);
    }
}

void ChartWidget::connectViewModel() {
    if (!viewModel_) return;

    // Connect ViewModel signals
    connect(viewModel_, &ChartViewModel::dataLoaded, this, &ChartWidget::onDataLoaded);
    connect(viewModel_, &ChartViewModel::dataLoadFailed, this, &ChartWidget::onDataLoadFailed);

    // Sync symbol input with ViewModel
    connect(symbolEdit_, &QLineEdit::textChanged, [this](const QString& text) {
        if (viewModel_) {
            viewModel_->setSymbol(text.trimmed().toUpper());
        }
    });
}

void ChartWidget::onSymbolChanged() {
    if (viewModel_) {
        QString symbol = symbolEdit_->text().trimmed().toUpper();
        viewModel_->setSymbol(symbol);
    }
}

void ChartWidget::onRefreshClicked() {
    if (viewModel_) {
        viewModel_->refreshData(currentInterval_);
    }
}

void ChartWidget::onTimeIntervalChanged(int id) {
    currentInterval_ = static_cast<TimeInterval>(id);
    updateTimeAxis();

    if (viewModel_) {
        viewModel_->loadData(currentInterval_);
    }
}

void ChartWidget::onDataLoaded(const QVector<OHLCV>& data) {
    displayData(data);
}

void ChartWidget::onDataLoadFailed(const QString& error) {
    QMessageBox::warning(this, tr("Data Load Error"), error);
}

void ChartWidget::updateTimeAxis() {
    QCPAxisRect *volumeAxisRect = customPlot_->axisRect(1);
    QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
    dateTimeTicker->setDateTimeSpec(Qt::UTC);

    // Set format based on time interval
    switch (currentInterval_) {
        case TimeInterval::Second:
            dateTimeTicker->setDateTimeFormat("hh:mm:ss");
            break;
        case TimeInterval::Minute:
            dateTimeTicker->setDateTimeFormat("hh:mm");
            break;
        case TimeInterval::Hour:
            dateTimeTicker->setDateTimeFormat("MMM dd hh:mm");
            break;
        case TimeInterval::Day:
            dateTimeTicker->setDateTimeFormat("MMM dd");
            break;
        case TimeInterval::Month:
            dateTimeTicker->setDateTimeFormat("yyyy MMM");
            break;
    }

    volumeAxisRect->axis(QCPAxis::atBottom)->setTicker(dateTimeTicker);
    customPlot_->xAxis->setTicker(dateTimeTicker);

    updatePriceAxisTicks();
}

void ChartWidget::updatePriceAxisTicks() {
    // Get current y-axis range
    QCPRange yRange = customPlot_->yAxis->range();
    double rangeSize = yRange.size();

    // Calculate optimal tick step based on price range
    double tickStep = 1.0;
    if (rangeSize < 5) {
        tickStep = 0.5;
    } else if (rangeSize < 10) {
        tickStep = 1.0;
    } else if (rangeSize < 50) {
        tickStep = 5.0;
    } else if (rangeSize < 100) {
        tickStep = 10.0;
    } else if (rangeSize < 500) {
        tickStep = 50.0;
    } else {
        tickStep = 100.0;
    }

    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    fixedTicker->setTickStep(tickStep);
    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
    customPlot_->yAxis->setTicker(fixedTicker);
}

void ChartWidget::contextMenuEvent(QContextMenuEvent* event) {
    if (drawingMenu_ && customPlot_->underMouse()) {
        drawingMenu_->exec(event->globalPos());
    } else {
        DomainWidget::contextMenuEvent(event);
    }
}

void ChartWidget::showDrawingMenu() {
    // This will be called when right-click on chart
    // Menu is already set up in setupUI()
}
