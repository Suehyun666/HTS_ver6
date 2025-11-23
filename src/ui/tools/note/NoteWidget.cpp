#include "NoteWidget.h"
#include <QVBoxLayout>

NoteWidget::NoteWidget(QWidget* parent) : DomainWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    textEdit = new QTextEdit(this);
    layout->addWidget(textEdit);
    setLayout(layout);
}
