#ifndef HTS_VER6_NOTEWIDGET_H
#define HTS_VER6_NOTEWIDGET_H

#include "../../widget/DomainWidget.h"
#include <QTextEdit>

class NoteWidget : public DomainWidget {
    Q_OBJECT
public:
    explicit NoteWidget(QWidget* parent = nullptr);

    QString windowId() const override { return "note"; }
    QString windowTitle() const override { return tr("Note"); }

protected:
    QSize preferredSize() const override { return QSize(400, 500); }
    QSize minimumSizeHint() const override { return QSize(300, 300); }
    QSize maximumSizeHint() const override { return QSize(800, 1000); }

private:
    QTextEdit* textEdit;
};

#endif
