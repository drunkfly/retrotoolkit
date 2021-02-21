#include "LogWidget.h"
#include "GUI/Util/Conversion.h"
#include <QScrollBar>

LogWidget::LogWidget(QWidget* parent)
    : QPlainTextEdit(parent)
{
    setUndoRedoEnabled(false);
    setReadOnly(true);
}

LogWidget::~LogWidget()
{
}

void LogWidget::print(const char* message)
{
    print(QString::fromUtf8(message));
}

void LogWidget::print(const std::string& message)
{
    print(fromUtf8(message));
}

void LogWidget::print(const QString& message)
{
    QScrollBar* scrollBar = verticalScrollBar();
    int scrollPos = scrollBar->value();
    bool wasAtBottom = (scrollPos >= scrollBar->maximum() - 4);

    setUpdatesEnabled(false);
    appendPlainText(message);
    moveCursor(QTextCursor::End);
    setUpdatesEnabled(true);

    if (wasAtBottom)
        ensureCursorVisible();
    else
        scrollBar->setValue(scrollPos);
}
