#ifndef GUI_WIDGETS_LOGWIDGET_H
#define GUI_WIDGETS_LOGWIDGET_H

#include "GUI/Common.h"
#include <QPlainTextEdit>

class LogWidget : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit LogWidget(QWidget* parent = nullptr);
    ~LogWidget() override;

    void clear();

    void print(const std::string& message);
    bool flush(bool force = false);

    void println(const QString& message);

private:
    std::vector<char> mBuffer;

    DISABLE_COPY(LogWidget);
};

#endif
