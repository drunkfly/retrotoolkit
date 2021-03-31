#include "LogWidget.h"
#include "GUI/Util/Conversion.h"

LogWidget::LogWidget(QWidget* parent)
    : QPlainTextEdit(parent)
{
    setUndoRedoEnabled(false);
    setReadOnly(true);
    mBuffer.reserve(1024);
}

LogWidget::~LogWidget()
{
}

void LogWidget::clear()
{
    QPlainTextEdit::clear();
    mBuffer.clear();
}

void LogWidget::print(const std::string& message)
{
    for (char ch : message) {
        if (ch != '\n')
            mBuffer.emplace_back(ch);
        else
            flush(true);
    }
}

bool LogWidget::flush(bool force)
{
    size_t n = mBuffer.size();
    if (n == 0) {
        if (!force)
            return false;
        else
            mBuffer.emplace_back(0);
    } else {
        if (mBuffer[n - 1] == '\r')
            mBuffer[n - 1] = 0;
        else
            mBuffer.emplace_back(0);
    }

    println(QString::fromUtf8(mBuffer.data()));
    mBuffer.clear();

    return true;
}

void LogWidget::println(const QString& message)
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
