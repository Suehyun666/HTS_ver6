#ifndef HTS_VER6_CHARTWIDGET_H
#define HTS_VER6_CHARTWIDGET_H
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QDate>
#include <QButtonGroup>
#include "../widget/DomainWidget.h"
#include "../plot/qcustomplot.h"
#include "../../domain/model/OHLCV.h"

class QLineEdit;
class QPushButton;
class QMenu;
class ChartViewModel;

class ChartWidget : public DomainWidget {
    Q_OBJECT
public:
    explicit ChartWidget(ChartViewModel* viewModel, QWidget* parent = nullptr);

    QString windowId() const override { return "chart"; }
    QString windowTitle() const override { return tr("Chart"); }

protected:
    QSize preferredSize() const override { return QSize(900, 600); }
    QSize minimumSizeHint() const override { return QSize(600, 400); }
    QSize maximumSizeHint() const override { return QSize(1600, 1200); }
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void onSymbolChanged();
    void onRefreshClicked();
    void onTimeIntervalChanged(int id);
    void showDrawingMenu();
    void onDataLoaded(const QVector<OHLCV>& data);
    void onDataLoadFailed(const QString& error);
    void onXRangeChanged(const QCPRange& newRange);
    void onYRangeChanged(const QCPRange& newRange);

private:
    void setupUI();
    void setupChart();
    void connectViewModel();
    void displayData(const QVector<OHLCV>& data);
    void updateTimeAxis();
    void updatePriceAxisTicks();

    ChartViewModel* viewModel_;
    QCustomPlot* customPlot_;
    QLineEdit* symbolEdit_;
    TimeInterval currentInterval_;
    QButtonGroup* timeIntervalGroup_;
    QMenu* drawingMenu_;

    QCPRange initialXRange_;
    QCPRange initialYRange_;
    int dataPointCount_;
};

#endif
