#include "WindowManager.h"
#include "../../ui/widget/DomainWidget.h"
#include "../../ui/order/OrderWidget.h"
#include "../../ui/chart/ChartWidget.h"
#include "../../ui/account/AccountView.h"
#include "../../ui/tools/note/NoteWidget.h"
#include "../../ui/tools/calculator/CalculatorWidget.h"
#include "../../ui/tools/calendar/CalendarWidget.h"
#include "../../ui/tools/clock/ClockWidget.h"
#include "../factory/ViewModelFactory.h"
#include "../../viewmodel/account/AccountManager.h"
#include <QDebug>

WindowManager::WindowManager(QMdiArea* mdiArea, QObject* parent)
    : QObject(parent), mdiArea(mdiArea) {
}

template<typename T>
QMdiSubWindow* WindowManager::openWindow() {
    T* widget = new T();
    DomainWidget* domainWidget = qobject_cast<DomainWidget*>(widget);

    if (!domainWidget) {
        delete widget;
        return nullptr;
    }

    if (domainWidget->isSingleton()) {
        QMdiSubWindow* existing = findWindow(domainWidget->windowId());
        if (existing) {
            delete widget;
            mdiArea->setActiveSubWindow(existing);
            return existing;
        }
    }

    QMdiSubWindow* sub = mdiArea->addSubWindow(domainWidget);
    sub->setWindowTitle(domainWidget->windowTitle());
    sub->setAttribute(Qt::WA_DeleteOnClose);

    connect(sub, &QMdiSubWindow::windowStateChanged, this,
        [domainWidget](Qt::WindowStates oldState, Qt::WindowStates newState) {
            if (newState & Qt::WindowActive) {
                domainWidget->onActivated();
            } else if (oldState & Qt::WindowActive) {
                domainWidget->onDeactivated();
            }
        });

    sub->show();
    return sub;
}

// Template specialization for OrderWidget
template<>
QMdiSubWindow* WindowManager::openWindow<OrderWidget>() {
    auto* viewModel = ViewModelFactory::instance().createOrderViewModel(this);
    auto* widget = new OrderWidget(viewModel);
    return addWidgetToMdi(widget);
}

// Template specialization for ChartWidget
template<>
QMdiSubWindow* WindowManager::openWindow<ChartWidget>() {
    auto* viewModel = ViewModelFactory::instance().createChartViewModel(this);
    auto* widget = new ChartWidget(viewModel);
    return addWidgetToMdi(widget);
}

// Template specialization for AccountView
template<>
QMdiSubWindow* WindowManager::openWindow<AccountView>() {
    auto* widget = new AccountView();

    // Get ViewModel from AccountManager
    AccountManager* accountManager = AccountManager::instance();
    if (!accountManager) {
        qWarning() << "[WindowManager] AccountManager instance is null!";
    } else {
        qint64 primaryId = accountManager->getPrimaryAccountId();
        qDebug() << "[WindowManager] Primary account ID:" << primaryId;

        if (primaryId > 0) {
            AccountViewModel* viewModel = accountManager->getAccountViewModel(primaryId);
            if (viewModel) {
                qDebug() << "[WindowManager] Found ViewModel for account" << primaryId;
                widget->setViewModel(viewModel);
            } else {
                qWarning() << "[WindowManager] No ViewModel found for account" << primaryId;
            }
        } else {
            qWarning() << "[WindowManager] Invalid primary account ID:" << primaryId;
        }
    }

    return addWidgetToMdi(widget);
}

QMdiSubWindow* WindowManager::addWidgetToMdi(QWidget* widget) {
    DomainWidget* domainWidget = qobject_cast<DomainWidget*>(widget);

    if (!domainWidget) {
        delete widget;
        return nullptr;
    }

    if (domainWidget->isSingleton()) {
        QMdiSubWindow* existing = findWindow(domainWidget->windowId());
        if (existing) {
            delete widget;
            mdiArea->setActiveSubWindow(existing);
            return existing;
        }
    }

    QMdiSubWindow* sub = mdiArea->addSubWindow(domainWidget);
    sub->setWindowTitle(domainWidget->windowTitle());
    sub->setAttribute(Qt::WA_DeleteOnClose);

    connect(sub, &QMdiSubWindow::windowStateChanged, this,
        [domainWidget](Qt::WindowStates oldState, Qt::WindowStates newState) {
            if (newState & Qt::WindowActive) {
                domainWidget->onActivated();
            } else if (oldState & Qt::WindowActive) {
                domainWidget->onDeactivated();
            }
        });

    sub->show();
    return sub;
}

template QMdiSubWindow* WindowManager::openWindow<OrderWidget>();
template QMdiSubWindow* WindowManager::openWindow<ChartWidget>();
template QMdiSubWindow* WindowManager::openWindow<AccountView>();
template QMdiSubWindow* WindowManager::openWindow<NoteWidget>();
template QMdiSubWindow* WindowManager::openWindow<CalculatorWidget>();
template QMdiSubWindow* WindowManager::openWindow<CalendarWidget>();
template QMdiSubWindow* WindowManager::openWindow<ClockWidget>();

QMdiSubWindow* WindowManager::findWindow(const QString& windowId) {
    for (QMdiSubWindow* sub : mdiArea->subWindowList()) {
        DomainWidget* widget = qobject_cast<DomainWidget*>(sub->widget());
        if (widget && widget->windowId() == windowId) {
            return sub;
        }
    }
    return nullptr;
}

void WindowManager::closeAll() {
    mdiArea->closeAllSubWindows();
}
